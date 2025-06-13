#include "WebXWindowShape.h"
#include <image/WebXImageConverter.h>
#include <image/WebXImage.h>
#include <X11/extensions/shape.h>
#include <spdlog/spdlog.h>
#include <crc32/Crc32.h>



WebXWindowShape::WebXWindowShape(Display * display, Window x11Window, int width, int height) :
    _display(display),
    _x11Window(x11Window),
    _width(width),
    _height(height),
    _isBuilt(false),
    _eventListenerEnabled(false),
    _shapeMask(nullptr),
    _rectanglesChecksum(0) {
}

WebXWindowShape::~WebXWindowShape() {
    if (this->_eventListenerEnabled) {
        XShapeSelectInput(this->_display,  this->_x11Window, 0);
    }
}

void WebXWindowShape::update(int width, int height, WebXImageConverter * imageConverter, const WebXQuality & quality, bool force) {
    if (force) {
        this->_isBuilt = false;
    }

    if (this->_isBuilt && this->_width == width && this->_height == height) {
        // No need to update if the dimensions are the same
        return;
    }

    // Update the width and height
    this->_width = width;
    this->_height = height;

    // Create the shape mask
    this->_isBuilt = false;
    this->_shapeMaskChecksum = 0;
    this->create(imageConverter, quality);

    if (!this->_eventListenerEnabled) {
        // Request events for shape updates
        XShapeSelectInput(this->_display,  this->_x11Window, ShapeNotifyMask);
        this->_eventListenerEnabled = true;
    }
}

void WebXWindowShape::create(WebXImageConverter * imageConverter, const WebXQuality & quality) {
    if (this->_isBuilt) {
        return;
    }

    // Test for shape
    int count, ordering;
    XRectangle * rects = XShapeGetRectangles(this->_display, this->_x11Window, ShapeBounding, &count, &ordering);
    if (!rects || count == 0 || (count == 1 && rects[0].x == 0 && rects[0].y == 0 && rects[0].width == this->_width && rects[0].height == this->_height)) {
        this->_isBuilt = true;
        return;
    }

    // Even if we get an event to update the shape, we check if the rectangles have changed before rebuilding the shape image.
    uint32_t rectanglesChecksum = crc32_16bytes((const uint8_t *)rects, count * sizeof(XRectangle));
    if (rectanglesChecksum == this->_rectanglesChecksum) {
        this->_isBuilt = true;
        return;
    }

    // Rectangles changed
    this->_rectanglesChecksum = rectanglesChecksum;

    // spdlog::info("Window 0x{:x} with size {:d}x{:d} has shape with {:d} rectangles", this->_x11Window, this->_width, this->_height, count);

    // Create an 8-bit Pixmap with the same size as the window
    Pixmap pixmap = XCreatePixmap(this->_display, this->_x11Window, this->_width, this->_height, 8);
    GC gc = XCreateGC(this->_display, pixmap, 0, NULL);

    // Clear to black (0)
    XSetForeground(this->_display, gc, 0);
    XFillRectangle(this->_display, pixmap, gc, 0, 0, this->_width, this->_height);

    // Fill shape area with white (255)
    XSetForeground(this->_display, gc, 255);
    for (int i = 0; i < count; i++) {
        XFillRectangle(this->_display, pixmap, gc,
                       rects[i].x,
                       rects[i].y,
                       rects[i].width,
                       rects[i].height);
    }

    // Read pixel data
    XImage * shapeImage = XGetImage(this->_display, pixmap, 0, 0, this->_width, this->_height, AllPlanes, ZPixmap);
    this->_shapeMask = std::shared_ptr<WebXImage>(imageConverter->convertMono(shapeImage, quality));
    this->_shapeMaskChecksum = this->_shapeMask->calculateImageChecksum();

    XDestroyImage(shapeImage);
    XFreePixmap(this->_display, pixmap);
    XFreeGC(this->_display, gc);
    XFree(rects);

    this->_isBuilt = true;
}