#ifndef WEBX_MOUSE_STATE_H
#define WEBX_MOUSE_STATE_H

#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>
#include <cstring>
#include <spdlog/spdlog.h>
#include "input/cursor/WebXMouseCursor.h"

/**
 * Represents the current mouse state
 */
class WebXMouseState {
public:
    WebXMouseState(std::shared_ptr<WebXMouseCursor> cursor) :
        _x(0), 
        _y(0), 
        _buttonMask(0), 
        _cursor(cursor) {
    }

    virtual ~WebXMouseState() {
    }

    /**
     * Update the new mouse state
     * @param x the x position relative to the root window
     * @param y the y position relative to the root window
     * @param buttonMask the button mask
     */
    void setState(int x, int y, int buttonMask) {
        _x = x;
        _y = y;
        _buttonMask = buttonMask;
    }

    /**
     * Update the new mouse position
     * @param x the x position relative to the root window
     * @param y the y position relative to the root window
     */
    void setPosition(int x, int y) {
        _x = x;
        _y = y;
    }

    /**
     * Get the x position relative to the root window
     * @return the x position
     */
    int getX() const {
        return _x;
    }

    /**
    * Get the x position relative to the root window
    * @return the y position
    */
    int getY() const {
        return _y;
    }

    /**
     * Get the current button mask
     * @return the mask
     */
    int getButtonMask() const {
        return _buttonMask;
    }

    void setButtonMask(int buttonMask) {
        this->_buttonMask = buttonMask;
    }
    
    std::shared_ptr<WebXMouseCursor> getCursor() const {
        return _cursor;
    }

    void setCursor(std::shared_ptr<WebXMouseCursor> newCursor) {
        _cursor = newCursor;
    }

private:
    int _x;
    int _y;
    int _buttonMask;
    std::shared_ptr<WebXMouseCursor> _cursor;
};

#endif //WEBX_MOUSE_STATE_H
