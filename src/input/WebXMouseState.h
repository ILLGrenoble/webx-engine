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
    WebXMouseState(WebXMouseCursor * cursor) :
        _x(0), _y(0), _buttonMask(0), _cursor(cursor), _previousCursor(nullptr) {
    }

    ~WebXMouseState() {
        delete _cursor;
        if (_previousCursor) {
            delete _previousCursor;
        }
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
    
    WebXMouseCursor * getCursor() const {
        return _cursor;
    }

    /**
     * Check if the current cursor is different from the previous cursor
     * @return true if different, false if not
     */
    bool isCursorDifferent() const {
        if (_cursor) {
            if (_previousCursor) {
                return (_cursor->getInfo()->pixels != _previousCursor->getInfo()->pixels);
            }
        } else {
            if (!_previousCursor) {
                return true;
            }
        }
        return false;
    }

    void setCursor(WebXMouseCursor * newCursor) {
        if (_previousCursor) {
            delete _previousCursor;
            _previousCursor = nullptr;
        }
        _previousCursor = _cursor;
        _cursor = newCursor;
    }

private:
    int _x;
    int _y;
    int _buttonMask;
    WebXMouseCursor * _cursor;
    WebXMouseCursor * _previousCursor;
};

#endif //WEBX_MOUSE_STATE_H
