#include "WebXMouse.h"
#include "WebXMouseState.h"
#include "cursor/WebXMouseCursorFactory.h"
#include <X11/X.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/XTest.h>

WebXMouse::WebXMouse(Display * x11Display, Window rootWindow) :
    _x11Display(x11Display),
    _rootWindow(rootWindow),
    _cursorFactory(new WebXMouseCursorFactory(x11Display)),
    _currentMouseState(createDefaultMouseState()) {
}

WebXMouse::~WebXMouse() {
    delete _cursorFactory;
    delete _currentMouseState;
}

void WebXMouse::sendClientInstruction(int x, int y, unsigned int buttonMask) {
    sendMouseMovement(x, y);
    sendMouseButtons(buttonMask);
    _currentMouseState->setState(x, y, buttonMask);
}

void WebXMouse::sendMouseButtons(unsigned int newButtonMask) {
    int currentButtonMask = _currentMouseState->getButtonMask();
    int buttonMaskDelta = currentButtonMask ^newButtonMask;
    unsigned int buttonMasks[5] = {LeftButtonMask, MiddleButtonMask, RightButtonMask, ScrollUpButtonMask, ScrollDownButtonMask};
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

void WebXMouse::updateCursor() {
    std::shared_ptr<WebXMouseCursor> cursor = this->_cursorFactory->createCursor();
    if (cursor) {
        _currentMouseState->setCursor(cursor);
    }
}

WebXMouseState * WebXMouse::createDefaultMouseState() {
    std::shared_ptr<WebXMouseCursor> cursor = this->_cursorFactory->createCursor();
    return new WebXMouseState(cursor);
}





