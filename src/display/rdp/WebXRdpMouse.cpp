#include "WebXRdpMouse.h"
#include "WebXRdpPointer.h"
#include <display/input/WebXMouseState.h>

WebXRdpMouse::WebXRdpMouse(WebXRdpPointer * rdpPointer) :
    WebXMouse(),
    _rdpPointer(rdpPointer),
    _state(createDefaultMouseState()),
    _isDirty(false) {
}

WebXRdpMouse::~WebXRdpMouse() {
    delete _state;
}

void WebXRdpMouse::sendClientInstruction(int x, int y, unsigned int buttonMask) {
    sendMouseMovement(x, y);
    sendMouseButtons(buttonMask);
    _state->setState(x, y, buttonMask);

    // Force an update of the position (read from X11) so that we can be sure that the state is stable
    this->updatePosition();
    this->_isDirty = false;
}

void WebXRdpMouse::sendMouseButtons(unsigned int newButtonMask) {
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

void WebXRdpMouse::sendMouseButton(unsigned int button, bool isPressed) {
    this->_rdpPointer->sendButton(button, isPressed);
}

void WebXRdpMouse::sendMouseMovement(int newX, int newY) {
    int currentX = _state->getX();
    int currentY = _state->getY();
    if (newX != currentX || newY != currentY) {
        this->_rdpPointer->sendPosition(newX, newY);
    }
}

void WebXRdpMouse::updateCursor() {
    std::shared_ptr<WebXMouseCursor> cursor = this->_rdpPointer->cursor();
    if (cursor) {
        _state->setCursor(cursor);
    }
}

std::shared_ptr<WebXMouseCursor> WebXRdpMouse::getCursor(uint32_t cursorId) {
    if (cursorId == 0) {
        return this->_state->getCursor();
    
    } else {
        return this->_rdpPointer->cursorWithId(cursorId);
    }
}

void WebXRdpMouse::updatePosition() {
    // Get the mouse cursor position
    uint32_t x = this->_rdpPointer->x();
    uint32_t y = this->_rdpPointer->y();

    // Check if the position has changed
    if (this->_state->getX() != x || this->_state->getY() != y) {
        this->_state->setPosition(x, y);
        this->_isDirty = true;
    }
}

WebXMouseState * WebXRdpMouse::createDefaultMouseState() {
    return new WebXMouseState(nullptr);
}





