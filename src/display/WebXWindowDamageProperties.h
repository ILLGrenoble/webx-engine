#ifndef WEBX_WINDOW_DAMAGE_PROPERTIES_H
#define WEBX_WINDOW_DAMAGE_PROPERTIES_H

#include "WebXWindow.h"
#include <utils/WebXSize.h>
#include <utils/WebXRectangle.h>

class WebXWindowDamageProperties {
public:
    WebXWindowDamageProperties(WebXWindow * window, const WebXRectangle & damageArea, bool fullWindow = false) :
        windowId((unsigned long)window->getX11Window()),
        windowSize(window->getRectangle().size),
        fullWindow(fullWindow),
        imageCaptureTime(window->getImageCaptureTime()) {
        this->damageAreas.push_back(damageArea);
    }
    WebXWindowDamageProperties(const WebXWindowDamageProperties & windowDamage) :
        windowId(windowDamage.windowId),
        windowSize(windowDamage.windowSize),
        damageAreas(windowDamage.damageAreas),
        fullWindow(windowDamage.fullWindow),
        imageCaptureTime(windowDamage.imageCaptureTime) {
    }
    virtual ~WebXWindowDamageProperties() {
    }

    WebXWindowDamageProperties & operator=(const WebXWindowDamageProperties & windowDamage) {
        if (this != &windowDamage) {
            this->windowId = windowDamage.windowId;
            this->windowSize = windowDamage.windowSize;
            this->damageAreas = windowDamage.damageAreas;
            this->fullWindow = windowDamage.fullWindow;
            this->imageCaptureTime = windowDamage.imageCaptureTime;
        }
        return *this;
    }

    WebXWindowDamageProperties & operator+=(const WebXRectangle & damageArea) {

        WebXRectangle combinedDamageArea = damageArea;

        auto it = this->damageAreas.begin();
        while (it != this->damageAreas.end()) {
            WebXRectangle & existingDamageArea = *it;
            if (existingDamageArea.overlapOrTouhcing(combinedDamageArea)) {
                combinedDamageArea += existingDamageArea;
                it = this->damageAreas.erase(it);
            
            } else {
                it++;
            }
        }

        this->damageAreas.push_back(combinedDamageArea);

        return *this;
    }

    bool isFullWindow() const {
        if (this->fullWindow) {
            return true;
        }
        if (this->damageAreas.size() != 1) {
            return false;
        }
        const WebXRectangle & damageArea = this->damageAreas[0];
        return damageArea.size.width == windowSize.width && damageArea.size.height == windowSize.height;
    }

    unsigned long windowId;
    WebXSize windowSize;
    std::vector<WebXRectangle> damageAreas;
    bool fullWindow;
    std::chrono::high_resolution_clock::time_point imageCaptureTime;
};


#endif /* WEBX_WINDOW_PROPERTIES */