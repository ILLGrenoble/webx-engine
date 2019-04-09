#ifndef WEBX_WINDOW_PROPERTIES_H
#define WEBX_WINDOW_PROPERTIES_H

#include "WebXWindow.h"

class WebXWindowProperties {
public:
    WebXWindowProperties(WebXWindow * window) :
        id((unsigned long)window->getX11Window()),
        x(window->getRectangle().x),
        y(window->getRectangle().y),
        width(window->getRectangle().width),
        height(window->getRectangle().height),
        bpp(window->getBitsPerPixel()),
        imageCaptureTime(window->getImageCaptureTime()) {
    }
    WebXWindowProperties(const WebXWindowProperties & window) :
        id(window.id),
        x(window.x),
        y(window.y),
        width(window.width),
        height(window.height),
        bpp(window.bpp),
        imageCaptureTime(window.imageCaptureTime) {
    }
    virtual ~WebXWindowProperties() {}

    WebXWindowProperties & operator=(const WebXWindowProperties & window) {
        if (this != &window) {
            this->id = window.id;
            this->x = window.x;
            this->y = window.y;
            this->width = window.width;
            this->height = window.height;
            this->bpp = window.bpp;
            this->imageCaptureTime = window.imageCaptureTime;
        }
        return *this;
    }

    unsigned long id;
    unsigned int bpp;
    int x;
    int y;
    int width;
    int height;
    std::chrono::high_resolution_clock::time_point imageCaptureTime;
};


#endif /* WEBX_WINDOW_PROPERTIES */