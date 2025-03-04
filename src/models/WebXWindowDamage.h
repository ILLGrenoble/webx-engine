#ifndef WEBX_WINDOW_DAMAGE_H
#define WEBX_WINDOW_DAMAGE_H

#include <X11/Xlib.h>
#include <vector>
#include "WebXRectangle.h"

class WebXWindowDamage {
public:
    WebXWindowDamage(Window x11Window) :
        _x11Window(x11Window),
        _isFullWindow(false) {
    }
    WebXWindowDamage(Window x11Window, const WebXRectangle & damageArea, bool fullWindow = false) :
        _x11Window(x11Window),
        _isFullWindow(fullWindow) {
        this->_damageAreas.push_back(damageArea);
    }
    WebXWindowDamage(const WebXWindowDamage & windowDamage) :
        _x11Window(windowDamage._x11Window),
        _damageAreas(windowDamage._damageAreas),
        _isFullWindow(windowDamage._isFullWindow) {
    }
    virtual ~WebXWindowDamage() {
    }

    WebXWindowDamage & operator=(const WebXWindowDamage & windowDamage) {
        if (this != &windowDamage) {
            this->_x11Window = windowDamage._x11Window;
            this->_damageAreas = windowDamage._damageAreas;
            this->_isFullWindow = windowDamage._isFullWindow;
        }
        return *this;
    }

    WebXWindowDamage & operator+=(const WebXWindowDamage & windowDamage) {
        if (this->_isFullWindow) {
            // If already full then ignore additional damaged areas

        } else if (windowDamage._isFullWindow) {
            // If window data to be added is full then set this one to full
            this->_damageAreas.clear();
            this->_isFullWindow = true;

        } else {
            // Add all new damaged areas to this one
            for (const WebXRectangle & rect : windowDamage._damageAreas) {
                this->addDamageRecange(rect);
            }

        }
        return *this;
    }

    Window getX11Window() const {
        return this->_x11Window;
    }

    bool isFullWindow() const {
        return this->_isFullWindow;
    }

    bool isFullWindow(const WebXSize & windowSize) const {
        if (this->_isFullWindow) {
            return true;
        }
        if (this->_damageAreas.size() != 1) {
            return false;
        }
        const WebXRectangle & damageArea = this->_damageAreas[0];
        return damageArea.size().width() == windowSize.width() && damageArea.size().height() == windowSize.height();
    }

    const std::vector<WebXRectangle> & getDamagedAreas() const {
        return this->_damageAreas;
    }

    int getDamagedArea() const {
        int area = 0;
        for (const WebXRectangle & damageArea : this->_damageAreas) {
            area += damageArea.area();
        }
        return area;
    }

    bool hasDamage() const {
        return this->_isFullWindow || this->_damageAreas.size() > 0;
    }

    void reset() {
        this->_isFullWindow = false;
        this->_damageAreas.clear();
    }

private: 
    void addDamageRecange(const WebXRectangle & damageArea) {
        if (this->_isFullWindow) {
            return;
        }

        WebXRectangle combinedDamageArea = damageArea;

        auto it = this->_damageAreas.begin();
        while (it != this->_damageAreas.end()) {
            WebXRectangle & existingDamageArea = *it;
            if (existingDamageArea.overlapOrTouching(combinedDamageArea)) {
                combinedDamageArea += existingDamageArea;
                it = this->_damageAreas.erase(it);
            
            } else {
                it++;
            }
        }

        this->_damageAreas.push_back(combinedDamageArea);
    }    

private:
    Window _x11Window;
    std::vector<WebXRectangle> _damageAreas;
    bool _isFullWindow;
};


#endif /* WEBX_WINDOW_DAMAGE_H */