#include "WebXMouse.h"
#include "WebXMouseState.h"
#include <X11/X.h>
#include <X11/extensions/XTest.h>
#include <input/cursor/WebXMouseCursorImageConverter.h>

WebXMouse::WebXMouse(Display * x11Display, const Window &rootWindow) :
    _x11Display(x11Display),
    _rootWindow(rootWindow),
    _currentMouseState(createDefaultMouseState()) {
}

WebXMouse::~WebXMouse() {
    delete _currentMouseState;
}

void WebXMouse::updateMouse(int x, int y, unsigned int buttonMask) {
    sendMouseMovement(x, y);
    sendMouseButtons(buttonMask);
    updateMouseState(x, y, buttonMask);
}

void WebXMouse::sendMouseButtons(unsigned int newButtonMask) {
    int currentButtonMask = _currentMouseState->getButtonMask();
    int buttonMaskDelta = currentButtonMask ^newButtonMask;
    unsigned int buttonMasks[5] = {LeftButtonMask, MiddleButtonMask, RightButtonMask, ScrollUpButtonMask,
                                   ScrollDownButtonMask};
    unsigned int buttons[5] = {LeftButton, MiddleButton, RightButton, ScrollUpButton, ScrollDownButton};
    for (int i = 0; i <= 4; i++) {
        // Check if the state has changed for a given button
        if (buttonMaskDelta & buttonMasks[i]) {
            // update the button if changed
            sendMouseButton(buttons[i], newButtonMask & buttonMasks[i]);
        }
    }
    // reset the mask
    _currentMouseState->setButtonMask(0);
}

void WebXMouse::sendMouseButton(unsigned int button, Bool isPressed) {
    XTestFakeButtonEvent(_x11Display, button, isPressed, 0);
}

void WebXMouse::sendMouseMovement(int newX, int newY) {
    int currentX = _currentMouseState->getX();
    int currentY = _currentMouseState->getY();
    if (newX != currentX || newY != currentY) {
        XWarpPointer(_x11Display, 0L, _rootWindow, 0, 0, 0, 0, newX, newY);
    }
}

void WebXMouse::updateMouseState(int newX, int newY, int newButtonMask) {
    _currentMouseState->setState(newX, newY, newButtonMask);
}

void WebXMouse::updateCursor() {
    WebXMouseCursor * newCursor = getCursor();
    if (newCursor) {
        _currentMouseState->setCursor(newCursor);
    }
}

WebXMouseCursor * WebXMouse::getCursor() {
    unsigned long last_serial = 0;
    XFixesCursorImage * cursorImage = XFixesGetCursorImage(_x11Display);
    if (cursorImage) {
        if (cursorImage->cursor_serial == last_serial) {
            return NULL;
        }
        return new WebXMouseCursor(cursorImage);
    } else {
        XFree(cursorImage);
    }
    return NULL;
}

WebXMouseState * WebXMouse::createDefaultMouseState() {
    WebXMouseCursor * cursor = getCursor();
    return new WebXMouseState(cursor);
}

WebXMouseState * WebXMouse::getState() {
    return _currentMouseState;
}







