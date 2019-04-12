#ifndef WEBX_WINDOW_DAMAGE_PROPERTIES_H
#define WEBX_WINDOW_DAMAGE_PROPERTIES_H

#include "WebXWindow.h"
#include <utils/WebXRectangle.h>

class WebXWindowDamageProperties {
public:
    WebXWindowDamageProperties(WebXWindow * window, const WebXRectangle & damageArea) :
        windowId((unsigned long)window->getX11Window()),
        x(damageArea.x),
        y(damageArea.y),
        width(damageArea.width),
        height(damageArea.height),
        imageCaptureTime(window->getImageCaptureTime()) {
    }
    WebXWindowDamageProperties(const WebXWindowDamageProperties & windowDamage) :
        windowId(windowDamage.windowId),
        x(windowDamage.x),
        y(windowDamage.y),
        width(windowDamage.width),
        height(windowDamage.height),
        imageCaptureTime(windowDamage.imageCaptureTime) {
    }
    virtual ~WebXWindowDamageProperties() {}

    WebXWindowDamageProperties & operator=(const WebXWindowDamageProperties & windowDamage) {
        if (this != &windowDamage) {
            this->windowId = windowDamage.windowId;
            this->x = windowDamage.x;
            this->y = windowDamage.y;
            this->width = windowDamage.width;
            this->height = windowDamage.height;
            this->imageCaptureTime = windowDamage.imageCaptureTime;
        }
        return *this;
    }

    unsigned long windowId;
    int x;
    int y;
    int width;
    int height;
    std::chrono::high_resolution_clock::time_point imageCaptureTime;
};


#endif /* WEBX_WINDOW_PROPERTIES */