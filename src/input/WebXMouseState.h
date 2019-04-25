#ifndef WEBX_MOUSE_STATE_H
#define WEBX_MOUSE_STATE_H

#include <X11/Xlib.h>

/**
 * Represents the current mouse state
 */
class WebXMouseState {
public:
    WebXMouseState() :
        _x(0), _y(0), _buttonMask(0) {
    }

    virtual ~WebXMouseState();

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
    int getX() {
        return _x;
    }

    /**
    * Get the x position relative to the root window
    * @return the y position
    */
    int getY() {
        return _y;
    }

    /**
     * Get the current button mask
     * @return the mask
     */
    int getButtonMask() {
        return _buttonMask;
    }

private:
    int _x;
    int _y;
    int _buttonMask;
};

#endif //WEBX_MOUSE_STATE_H
