#ifndef WEBX_WINDOW_DAMAGE_H
#define WEBX_WINDOW_DAMAGE_H

#include <X11/Xlib.h>
#include <vector>
#include "WebXRectangle.h"

/**
 * @class WebXWindowDamage
 * @brief Represents damage (changes) to an X11 window, including damaged areas and full-window damage state.
 */
class WebXWindowDamage {
public:
    /**
     * @brief Constructs a WebXWindowDamage object for a specific X11 window.
     * @param x11Window The X11 window handle.
     */
    WebXWindowDamage(Window x11Window) :
        _x11Window(x11Window),
        _isFullWindow(false) {
    }

    /**
     * @brief Constructs a WebXWindowDamage object with a specific damage area.
     * @param x11Window The X11 window handle.
     * @param damageArea The rectangle representing the damaged area.
     * @param fullWindow Whether the entire window is damaged.
     */
    WebXWindowDamage(Window x11Window, const WebXRectangle & damageArea, bool fullWindow = false) :
        _x11Window(x11Window),
        _isFullWindow(fullWindow) {
        this->_damageAreas.push_back(damageArea);
    }

    /**
     * @brief Copy constructor for WebXWindowDamage.
     * @param windowDamage The object to copy from.
     */
    WebXWindowDamage(const WebXWindowDamage & windowDamage) :
        _x11Window(windowDamage._x11Window),
        _damageAreas(windowDamage._damageAreas),
        _isFullWindow(windowDamage._isFullWindow) {
    }

    /**
     * @brief Destructor for WebXWindowDamage.
     */
    virtual ~WebXWindowDamage() {
    }

    /**
     * @brief Assignment operator for WebXWindowDamage.
     * @param windowDamage The object to assign from.
     * @return A reference to the updated WebXWindowDamage object.
     */
    WebXWindowDamage & operator=(const WebXWindowDamage & windowDamage) {
        if (this != &windowDamage) {
            this->_x11Window = windowDamage._x11Window;
            this->_damageAreas = windowDamage._damageAreas;
            this->_isFullWindow = windowDamage._isFullWindow;
        }
        return *this;
    }

    /**
     * @brief Adds damage from another WebXWindowDamage object.
     * @param windowDamage The object containing additional damage information.
     * @return A reference to the updated WebXWindowDamage object.
     */
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

    /**
     * @brief Gets the X11 window handle.
     * @return The X11 window handle.
     */
    Window getX11Window() const {
        return this->_x11Window;
    }

    /**
     * @brief Checks if the entire window is damaged.
     * @return True if the entire window is damaged, false otherwise.
     */
    bool isFullWindow() const {
        return this->_isFullWindow;
    }

    /**
     * @brief Checks if the entire window is damaged based on its size.
     * @param windowSize The size of the window.
     * @return True if the entire window is damaged, false otherwise.
     */
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

    /**
     * @brief Gets the list of damaged areas.
     * @return A reference to a vector of WebXRectangle objects representing damaged areas.
     */
    const std::vector<WebXRectangle> & getDamagedAreas() const {
        return this->_damageAreas;
    }

    /**
     * @brief Calculates the total damaged area.
     * @return The total damaged area in pixels.
     */
    int getDamagedArea() const {
        int area = 0;
        for (const WebXRectangle & damageArea : this->_damageAreas) {
            area += damageArea.area();
        }
        return area;
    }

    /**
     * @brief Checks if there is any damage to the window.
     * @return True if there is damage, false otherwise.
     */
    bool hasDamage() const {
        return this->_isFullWindow || this->_damageAreas.size() > 0;
    }

    /**
     * @brief Resets the damage state, clearing all damaged areas.
     */
    void reset() {
        this->_isFullWindow = false;
        this->_damageAreas.clear();
    }

private:
    /**
     * @brief Adds a damaged rectangle, merging it with existing damaged areas if they overlap or touch.
     * @param damageArea The rectangle representing the damaged area to add.
     */
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