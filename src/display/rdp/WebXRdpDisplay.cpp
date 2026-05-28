#include "WebXRdpDisplay.h"
#include "WebXRdpClient.h"
#include "WebXRdpMouse.h"
#include <image/WebXJPGImageConverter.h>
#include <image/WebXImage.h>

WebXRdpDisplay::WebXRdpDisplay(WebXRdpClient * rdpClient) :
    _rdpClient(rdpClient),
    _windowVisibility(this->getWindowVisibility()),
    _imageConverter(new WebXJPGImageConverter()),
    _mouse(nullptr),
    _framebuffer(std::unique_ptr<WebXPixelBuffer>(new WebXPixelBuffer(rdpClient->getFramebuffer()))) /* TMP */ {

}

WebXRdpDisplay::~WebXRdpDisplay() {

    if (this->_mouse) {
        delete this->_mouse;
        this->_mouse = NULL;
    }

}

void WebXRdpDisplay::init() {
    // Create framebuffer to copy rdp gdi buffer

    this->_mouse = new WebXRdpMouse(this->_rdpClient->getPointer());

}


void WebXRdpDisplay::updateFramebuffer(const std::vector<WebXRectangle> & invalidRectangles, const WebXPixelBuffer & framebuffer) {
    // Copy full display framewbuffer (need to do performance testing on full screen)
    // Alternatively do a rasterized update: 
    //   rdp client draws to a buffer, invalidated rectangled are rasterized and copied to second buffer used for webx client updates


    this->_framebuffer = std::unique_ptr<WebXPixelBuffer>(new WebXPixelBuffer(framebuffer));
}


const WebXSize WebXRdpDisplay::getScreenSize() const {
    return this->_rdpClient->getScreenSize();
}

const std::vector<const WebXWindowVisibility *> WebXRdpDisplay::getWindowVisiblities() {
    return { &this->_windowVisibility };
}

const std::vector<WebXWindowProperties> WebXRdpDisplay::getVisibleWindowsProperties() {
    return { this->getWindowProperties() };
}

std::shared_ptr<WebXImage> WebXRdpDisplay::getImage(unsigned long windowId, const WebXQuality & quality, const WebXRectangle * imageRectangle) {
    WebXSize desktopSize = this->_rdpClient->getScreenSize();

    // Initialise rectangle as full window
    WebXRectangle rectangle(0, 0, desktopSize.width(), desktopSize.height());
    bool isFull = true;

    // If image rectangle is specified, validate its size
    if (imageRectangle != NULL) {
        // Validate sub rectangle:
        if (!rectangle.contains(*imageRectangle)) {
            spdlog::debug("Image rectangle is outside desktop bounds");
            return nullptr;

        } else {
            rectangle = *imageRectangle;
            isFull = false;
        }
    }

    int bytesPerPixel = this->_framebuffer->colorDepth == 24 ? 3 : 4;
    int bytesOffset = rectangle.y() * this->_framebuffer->bytesPerLine + rectangle.x() * bytesPerPixel;
    WebXPixelBuffer pixelBuffer = { (char *)(this->_framebuffer->pixels + bytesOffset), rectangle.size().width(), rectangle.size().height(), this->_framebuffer->bytesPerLine, this->_framebuffer->colorDepth };

    std::shared_ptr<WebXImage> webXImage = std::shared_ptr<WebXImage>(this->_imageConverter->convert(&pixelBuffer, quality));

    return webXImage;
}

std::shared_ptr<WebXImage> WebXRdpDisplay::getWindowShapeMask(unsigned long windowId) {
    return nullptr;
}

WebXMouse * WebXRdpDisplay::getMouse() const {
    return this->_mouse;
}

void WebXRdpDisplay::updateMouseCursor() {
    this->_mouse->updateCursor();
}


void WebXRdpDisplay::sendClientMouseInstruction(int x, int y, unsigned int buttonMask) {
    this->_mouse->sendClientInstruction(x, y, buttonMask);
}

void WebXRdpDisplay::sendKeyboard(int keysym, bool pressed) {
}

bool WebXRdpDisplay::loadKeyboardLayout(const std::string & layoutOrName) {
    return true;
}

std::string WebXRdpDisplay::getKeyboardLayoutName() const {
    return "fr";
}

bool WebXRdpDisplay::canResizeScreen() const {
    return false;
}

void WebXRdpDisplay::resizeScreen(unsigned int width, unsigned int height) {

    // Update framebuffer

    // Update window visibility and properties
    this->_windowVisibility = this->getWindowVisibility();

}

WebXWindowVisibility WebXRdpDisplay::getWindowVisibility() const {
    WebXSize screenSize = this->getScreenSize();

    return WebXWindowVisibility(0, WebXRectangle(0, 0, screenSize.width(), screenSize.height()), true);
}

WebXWindowProperties WebXRdpDisplay::getWindowProperties() const {
    WebXSize screenSize = this->getScreenSize();

    return WebXWindowProperties(0, 0, 0, screenSize.width(), screenSize.height(), false);
}
