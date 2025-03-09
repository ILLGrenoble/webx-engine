#include "WebXMouse.h"
#include "WebXMouseState.h"
#include <X11/X.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/XTest.h>

WebXMouse::WebXMouse(Display * x11Display, Window rootWindow) :
    _x11Display(x11Display),
    _rootWindow(rootWindow),
    _cursorFactory(x11Display),
    _state(createDefaultMouseState()) {
}

WebXMouse::~WebXMouse() {
    delete _state;
}

void WebXMouse::sendClientInstruction(int x, int y, unsigned int buttonMask) {
    sendMouseMovement(x, y);
    sendMouseButtons(buttonMask);
    _state->setState(x, y, buttonMask);
}

void WebXMouse::sendMouseButtons(unsigned int newButtonMask) {
    int currentButtonMask = _state->getButtonMask();
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
    _state->setButtonMask(0);
}

void WebXMouse::sendMouseButton(unsigned int button, Bool isPressed) {
    XTestFakeButtonEvent(_x11Display, button, isPressed, 0);
}

void WebXMouse::sendMouseMovement(int newX, int newY) {
    int currentX = _state->getX();
    int currentY = _state->getY();
    if (newX != currentX || newY != currentY) {
        XWarpPointer(_x11Display, 0L, _rootWindow, 0, 0, 0, 0, newX, newY);
    }
}

void WebXMouse::updateCursor() {
    std::shared_ptr<WebXMouseCursor> cursor = this->_cursorFactory.createCursor();
    if (cursor) {
        _state->setCursor(cursor);
    }
}

std::shared_ptr<WebXMouseCursor> WebXMouse::getCursor(uint32_t cursorId) {
    if (cursorId == 0) {
        return this->_state->getCursor();
    } else {
        return this->_cursorFactory.getCursor(cursorId);
    }
}

void WebXMouse::updatePosition(int x, int y) {
    this->_state->setPosition(x, y);
}

void WebXMouse::updatePosition() {
    // Get the mouse cursor position
    int win_x, win_y, root_x, root_y = 0;
    unsigned int mask = 0;
    Window child_win, root_win;
    XQueryPointer(this->_x11Display, this->_rootWindow, &child_win, &root_win, &root_x, &root_y, &win_x, &win_y, &mask);
    this->_state->setPosition(root_x, root_y);
}

WebXMouseState * WebXMouse::createDefaultMouseState() {
    std::shared_ptr<WebXMouseCursor> cursor = this->_cursorFactory.createCursor();
    return new WebXMouseState(cursor);
}





