#ifndef WEBX_MOUSE_STATE_H
#define WEBX_MOUSE_STATE_H

#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>

/**
 * Represents the current mouse state
 */
class WebXMouseState {
public:
    WebXMouseState(XFixesCursorImage * cursorImage) :
        _x(0), _y(0), _buttonMask(0), _cursorImage(NULL) {
        _cursorImage = cursorImage;
    }

    ~WebXMouseState() {
        if(_cursorImage != NULL) {
            delete _cursorImage;
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

    /**
     * Gets the current cursor image
     * @return the cursor image
     */
    XFixesCursorImage * getCursorImage() const {
        return _cursorImage;
    }

    void setCursorImage(XFixesCursorImage *cursorImage) {
        WebXMouseState::_cursorImage = cursorImage;
    }

private:
    int _x;
    int _y;
    int _buttonMask;
    XFixesCursorImage * _cursorImage;
};

#endif //WEBX_MOUSE_STATE_H
