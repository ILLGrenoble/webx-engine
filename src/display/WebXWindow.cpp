#include "WebXWindow.h"
#include <image/WebXImage.h>
#include <image/WebXImageAlphaConverter.h>
#include <algorithm>
#include <X11/Xutil.h>
#include "spdlog/spdlog.h"

/* CRC-32C (iSCSI) polynomial in reversed bit order. */
#define POLY 0x82f63b78

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
}

void WebXWindow::enableDamage() {
    tthread::lock_guard<tthread::mutex> lock(this->_damageMutex);
    if (this->_damage == 0) {
        // Raw damage
        this->_damage = XDamageCreate(this->_display, this->_x11Window, XDamageReportRawRectangles);
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
    if (status != BadWindow) {
        this->_rectangle = WebXRectangle(attr.x, attr.y, attr.width, attr.height);
        this->_isViewable = (attr.map_state == IsViewable);
    } else {
        this->_rectangle = WebXRectangle(0, 0, -1, -1);
        this->_isViewable = false;
    }
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
    if (imageRectangle != NULL) {
        rectangle = *imageRectangle;
    } else {
        rectangle = WebXRectangle(0, 0, this->_rectangle.size.width, this->_rectangle.size.height);
    }
    spdlog::debug("Grabbing WebXWindow 0x{:x} ({:d}, {:d}), {:d}x{:d})", this->_x11Window, rectangle.x, rectangle.y, rectangle.size.width, rectangle.size.height);
    XImage * image = XGetImage(this->_display, this->_x11Window, rectangle.x, rectangle.y, rectangle.size.width, rectangle.size.height, AllPlanes, ZPixmap);
    std::shared_ptr<WebXImage> webXImage = nullptr;

    if (image) {
        bool hasConvertedAlpha = WebXImageAlphaConverter::convert(image, &this->_rectangle, subWindowRectangle, &rectangle);
        webXImage = std::shared_ptr<WebXImage>(imageConverter->convert(image, hasConvertedAlpha));

        if (imageRectangle == NULL) {
            uint32_t checksum = this->calculateImageChecksum(image);
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

uint32_t WebXWindow::calculateImageChecksum(XImage * image) {
    spdlog::debug("Calculating checksum");
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    size_t length = (image->bytes_per_line * image->height) / 8;
    char *data = image->data;
    uint32_t crc = 0;
    crc = ~crc;
    while (length--) {
        crc ^= *data++;
        for (int k = 0; k < 8; k++) {
            crc = crc & 1 ? (crc >> 1) ^ POLY : crc >> 1;
        }
    }

    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> duration = end - start;
    spdlog::debug("Checksum for {:d} x {:d} in {:f}us", image->width, image->height, duration.count());
    return ~crc;
}


