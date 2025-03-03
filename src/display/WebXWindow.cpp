#include "WebXWindow.h"
#include "WebXErrorHandler.h"
#include <image/WebXImage.h>
#include <events/WebXDamageOverride.h>
#include <utils/WebXQuality.h>
#include <utils/WebXWindowImageUtils.h>
#include <algorithm>
#include <X11/Xutil.h>
#include <spdlog/spdlog.h>

WebXWindow::WebXWindow(Display * display, Window x11Window, bool isRoot, int x, int y, int width, int height, bool isViewable) :
    _display(display),
    _x11Window(x11Window),
    _damage(0),
    _isRoot(isRoot),
    _parent(NULL),
    _visibility(x11Window, WebXRectangle(x, y, width, height), isViewable) {
}

WebXWindow::~WebXWindow() {
    if (this->isViewable()) {
        // Disable damage events for the window
        this->disableDamage();
    
        //Disable all events from the window
        XSelectInput(this->_display, this->_x11Window, 0);
    }
}

void WebXWindow::enableDamage() {
    std::lock_guard<std::mutex> lock(this->_damageMutex);
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
    std::lock_guard<std::mutex> lock(this->_damageMutex);
    if (this->_damage != 0 && this->isViewable()) {
        XDamageDestroy(this->_display, this->_damage);
        this->_damage = 0;
    }
}

Status WebXWindow::updateAttributes() {
    XWindowAttributes attr;
    Status status = XGetWindowAttributes(this->_display, this->_x11Window, &attr);
    this->_visibility.setRectangle(WebXRectangle(attr.x, attr.y, attr.width, attr.height));
    this->_visibility.setViewable(attr.map_state == IsViewable && attr.c_class == InputOutput);

    return status;
}

void WebXWindow::printInfo() const {
    printf("WebXWindow = 0x%08lx [(%d, %d), %dx%d]\n", this->_x11Window, this->getRectangle().x(), this->getRectangle().y(), this->getRectangle().size().width(), this->getRectangle().size().height());
}

std::shared_ptr<WebXImage> WebXWindow::getImage(const WebXRectangle * imageRectangle, WebXImageConverter * imageConverter, const WebXQuality & quality) {

    // Update window attributes to ensure we can grab the pixels and the size is coherent
    Status status = this->updateAttributes();
    if (status == False) {
        spdlog::trace("WebXWindow 0x{:x} has been removed before getting an image", this->_x11Window);
        return nullptr;
    }

    // Initialise rectangle as full window
    WebXRectangle rectangle(0, 0, this->getRectangle().size().width(), this->getRectangle().size().height());
    bool isFull = true;

    // If image rectangle is specified, validate its size
    if (imageRectangle != NULL) {
        // Validate sub rectangle:
        if (!rectangle.contains(*imageRectangle)) {
            spdlog::debug("Image rectangle for WebXWindow 0x{:x} is outside window bounds", this->_x11Window);
            return nullptr;

        } else {
            rectangle = *imageRectangle;
            isFull = false;
        }
    }

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    // Fix for Ubuntu 20.04: xlib crashes if damage event occurs during XGetImage (specifically on a Chrome browser) 
    this->disableDamage();
    
    XImage * image = XGetImage(this->_display, this->_x11Window, rectangle.x(), rectangle.y(), rectangle.size().width(), rectangle.size().height(), AllPlanes, ZPixmap);
    std::shared_ptr<WebXImage> webXImage = nullptr;

    // Fix for Ubuntu 20.04
    this->enableDamage();

    std::chrono::high_resolution_clock::time_point grab = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> grabDuration = grab - start;
    
    if (image) {
        // Check if image has transparency and modify image depth accordingly
        bool hasTransparency = checkTransparent(image);
        image->depth = hasTransparency ? 32 : 24;

        webXImage = std::shared_ptr<WebXImage>(imageConverter->convert(image, quality));

        XDestroyImage(image);

        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> encodeDuration = end - grab;
        std::chrono::duration<double, std::milli> duration = end - start;

        spdlog::trace("Grabbed WebXWindow 0x{:x} ({:d}, {:d}), {:d} x {:d} ({:s}) in {:.2f}ms (grab = {:.2f}ms, encoding = {:.2f}ms)", this->_x11Window, rectangle.x(), rectangle.y(), rectangle.size().width(), rectangle.size().height(), (isFull ? "full window" : "sub window"), duration.count(), grabDuration.count(), encodeDuration.count());

    } else {
        // See if ErrorHandler has this window as it's last error source and determine exact error
        if (WebXErrorHandler::getLastErrorWindow()) {
            unsigned char lastError = WebXErrorHandler::getLastErrorCode();
            if (lastError == BadWindow || lastError == BadDrawable) {
                spdlog::warn("WebXWindow 0x{:x} has been removed while getting an image", this->_x11Window);
                return nullptr;

            } else if (lastError == BadMatch) {
                spdlog::warn("Failed to get image for window 0x{:x}: requested rectangle is outside window bounds", this->_x11Window);
                return nullptr;

            } else {
                spdlog::warn("Failed to get image for window 0x{:x}: error 0x{:02x}", this->_x11Window, lastError);
            }
        }
    }

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



