#ifndef WEBX_MOUSE_STATE_H
#define WEBX_MOUSE_STATE_H

#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>
#include <cstring>
#include <spdlog/spdlog.h>
#include "cursor/WebXMouseCursor.h"

/**
 * Represents the current mouse state, including position, button mask, and cursor.
 */
class WebXMouseState {
public:
    /**
     * Constructor for WebXMouseState.
     * @param cursor A shared pointer to the WebXMouseCursor object.
     */
    WebXMouseState(std::shared_ptr<WebXMouseCursor> cursor) :
        _x(0), 
        _y(0), 
        _buttonMask(0), 
        _cursor(cursor) {
    }

    /**
     * Destructor for WebXMouseState.
     */
    virtual ~WebXMouseState() {
    }

    /**
     * Update the mouse state with new position and button mask.
     * @param x The x position relative to the root window.
     * @param y The y position relative to the root window.
     * @param buttonMask The button mask representing the state of mouse buttons.
     */
    void setState(int x, int y, int buttonMask) {
        _x = x;
        _y = y;
        _buttonMask = buttonMask;
    }

    /**
     * Update the mouse position.
     * @param x The x position relative to the root window.
     * @param y The y position relative to the root window.
     */
    void setPosition(int x, int y) {
        _x = x;
        _y = y;
    }

    /**
     * Get the x position of the mouse relative to the root window.
     * @return The x position.
     */
    int getX() const {
        return _x;
    }

    /**
     * Get the y position of the mouse relative to the root window.
     * @return The y position.
     */
    int getY() const {
        return _y;
    }

    /**
     * Get the current button mask of the mouse.
     * @return The button mask.
     */
    int getButtonMask() const {
        return _buttonMask;
    }

    /**
     * Set the button mask for the mouse.
     * @param buttonMask The new button mask.
     */
    void setButtonMask(int buttonMask) {
        this->_buttonMask = buttonMask;
    }
    
    /**
     * Get the current cursor associated with the mouse.
     * @return A shared pointer to the WebXMouseCursor object.
     */
    std::shared_ptr<WebXMouseCursor> getCursor() const {
        return _cursor;
    }

    /**
     * Set a new cursor for the mouse.
     * @param newCursor A shared pointer to the new WebXMouseCursor object.
     */
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
