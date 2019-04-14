#ifndef WEBX_WINDOW_DAMAGE_PROPERTIES_H
#define WEBX_WINDOW_DAMAGE_PROPERTIES_H

#include "WebXWindow.h"
#include <utils/WebXRectangle.h>

class WebXWindowDamageProperties {
public:
    WebXWindowDamageProperties(WebXWindow * window, const WebXRectangle & damageArea) :
        windowId((unsigned long)window->getX11Window()),
        imageCaptureTime(window->getImageCaptureTime()){
        this->damageAreas.push_back(damageArea);
    }
    WebXWindowDamageProperties(const WebXWindowDamageProperties & windowDamage) :
        windowId(windowDamage.windowId),
        damageAreas(windowDamage.damageAreas),
        imageCaptureTime(windowDamage.imageCaptureTime) {
    }
    virtual ~WebXWindowDamageProperties() {
    }

    WebXWindowDamageProperties & operator=(const WebXWindowDamageProperties & windowDamage) {
        if (this != &windowDamage) {
            this->windowId = windowDamage.windowId;
            this->damageAreas = windowDamage.damageAreas;
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

    unsigned long windowId;
    std::vector<WebXRectangle> damageAreas;
    std::chrono::high_resolution_clock::time_point imageCaptureTime;
};


#endif /* WEBX_WINDOW_PROPERTIES */