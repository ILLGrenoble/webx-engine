#include "WebXWindow.h"
#include <image/WebXImage.h>
#include <image/WebXImageAlphaConverter.h>
#include <events/WebXDamageOverride.h>
#include <algorithm>
#include <X11/Xutil.h>
#include <spdlog/spdlog.h>

/* CRC-32C (iSCSI) polynomial in reversed bit order. */
#define POLY 0x82f63b78

#undef get16bits
#if (defined(__GNUC__) && defined(__i386__)) || defined(__WATCOMC__) \
  || defined(_MSC_VER) || defined (__BORLANDC__) || defined (__TURBOC__)
#define get16bits(d) (*((const uint16_t *) (d)))
#endif

#if !defined (get16bits)
#define get16bits(d) ((((uint32_t)(((const uint8_t *)(d))[1])) << 8)\
                       +(uint32_t)(((const uint8_t *)(d))[0]) )
#endif

WebXWindow::WebXWindow(Display * display, Window x11Window, bool isRoot, int x, int y, int width, int height, bool isViewable) :
    _display(display),
    _x11Window(x11Window),
    _damage(0),
    _isRoot(isRoot),
    _parent(NULL),
    _rectangle(WebXRectangle(x, y, width, height)),
    _isViewable(isViewable),
    _imageCaptureTime(std::chrono::high_resolution_clock::now()),
    _windowChecksum(0) {
}

WebXWindow::~WebXWindow() {
    this->disableDamage();

    //Disable all events from the window
    XSelectInput(this->_display, this->_x11Window, 0);
}

void WebXWindow::enableDamage() {
    tthread::lock_guard<tthread::mutex> lock(this->_damageMutex);
    if (this->_damage == 0) {
        // Flush all pending events
        XSync(this->_display, false);

        // Set the damage override to ignore this window
        WebXDamageOverride::setWindowToIgnore(this->_x11Window);

        // Enable damage for this window (generates a lot of useless events)
        this->_damage = XDamageCreate(this->_display, this->_x11Window, XDamageReportRawRectangles);

        // Flush all events again (this time ignorred via the override)
        XSync(this->_display, false);

        // Disable the damage override
        WebXDamageOverride::setWindowToIgnore(0);
    }
}

void WebXWindow::disableDamage() {
    tthread::lock_guard<tthread::mutex> lock(this->_damageMutex);
    if (this->_damage != 0) {
        XDamageDestroy(this->_display, this->_damage);
        this->_damage = 0;
    }
}

void WebXWindow::updateAttributes() {
    XWindowAttributes attr;
    Status status = XGetWindowAttributes(this->_display, this->_x11Window, &attr);
    this->_rectangle = WebXRectangle(attr.x, attr.y, attr.width, attr.height);
    this->_isViewable = (attr.map_state == IsViewable && attr.c_class == InputOutput);
}

void WebXWindow::printInfo() const {
    printf("WebXWindow = 0x%08lx [(%d, %d), %dx%d]\n", this->_x11Window, this->_rectangle.x, this->_rectangle.y, this->_rectangle.size.width, this->_rectangle.size.height);
}

WebXWindow * WebXWindow::getTopParent() const {
    WebXWindow * child = (WebXWindow *)this;
    while (!child->parentIsRoot()) {
        child = this->getParent();
    }

    return child;
}

WebXRectangle WebXWindow::getSubWindowRectangle() const {
    const WebXRectangle & initialRectangle = this->getRectangle();
    const WebXWindow * child = this;
    WebXRectangle subWindowRectangle(0, 0, initialRectangle.size.width, initialRectangle.size.height);

    while (!child->parentIsRoot()) {
        const WebXRectangle & childRectangle = child->getRectangle();
        subWindowRectangle.x += childRectangle.x;
        subWindowRectangle.y += childRectangle.y;

        child = child->getParent();
    }

    return subWindowRectangle;
}

std::shared_ptr<WebXImage> WebXWindow::getImage(WebXRectangle * subWindowRectangle, WebXRectangle * imageRectangle, WebXImageConverter * imageConverter) {
    
    WebXRectangle rectangle;
    bool isFull = true;
    if (imageRectangle != NULL) {
        rectangle = *imageRectangle;
        isFull = false;

    } else {
        rectangle = WebXRectangle(0, 0, this->_rectangle.size.width, this->_rectangle.size.height);
    }

    // Fix for Ubuntu 20.04: xlib crashes if damage event occurs during XGetImage (specifically on a Chrome browser) 
    this->disableDamage();
    
    spdlog::debug("Grabbing WebXWindow 0x{:x} ({:d}, {:d}), {:d} x {:d} ({:s})", this->_x11Window, rectangle.x, rectangle.y, rectangle.size.width, rectangle.size.height, isFull ? "full window" : "sub window");
    XImage * image = XGetImage(this->_display, this->_x11Window, rectangle.x, rectangle.y, rectangle.size.width, rectangle.size.height, AllPlanes, ZPixmap);
    std::shared_ptr<WebXImage> webXImage = nullptr;

    // Fix for Ubuntu 20.04
    this->enableDamage();
    
    if (image) {
        bool hasConvertedAlpha = WebXImageAlphaConverter::convert(image, &this->_rectangle, subWindowRectangle, &rectangle);
        webXImage = std::shared_ptr<WebXImage>(imageConverter->convert(image, hasConvertedAlpha));

        if (imageRectangle == NULL) {
            uint32_t checksum = this->calculateImageChecksum(webXImage);
            this->_windowChecksum = checksum;
        }

        XDestroyImage(image);
    
    } else {
        spdlog::error("Failed to get image for window 0x{:x}", this->_x11Window);
    }
    this->_imageCaptureTime = std::chrono::high_resolution_clock::now();

    return webXImage;
}

void WebXWindow::addChild(WebXWindow * child) {
    std::vector<WebXWindow *>::iterator it = find(this->_children.begin(), this->_children.end(), child);
    if (it == this->_children.end()) {
        this->_children.push_back(child);
        child->setParent(this);
    }
}

void WebXWindow::removeChild(WebXWindow * child) {
    std::vector<WebXWindow *>::iterator it = find(this->_children.begin(), this->_children.end(), child);
    if (it != this->_children.end()) {
        this->_children.erase(it);

        child->setParent(NULL);
    }
}

uint32_t WebXWindow::calculateImageChecksum(std::shared_ptr<WebXImage> image) {
    spdlog::trace("Calculating window image checksum");
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    uint32_t checksum = image->getChecksum();

    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> duration = end - start;
    spdlog::trace("Checksum for window image {:d} x {:d} ({:d} bytes) in {:f}us", image->getWidth(), image->getHeight(), image->getRawDataSize(), duration.count());
    return checksum;
}


