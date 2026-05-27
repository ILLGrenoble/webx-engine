#include "WebXRdpMouse.h"
#include <display/input/WebXMouseState.h>

WebXRdpMouse::WebXRdpMouse(WebXRdpClient * rdpClient) :
    WebXMouse(),
    _rdpClient(rdpClient),
    _state(createDefaultMouseState()),
    _isDirty(false) {
}

WebXRdpMouse::~WebXRdpMouse() {
    delete _state;
}

void WebXRdpMouse::sendClientInstruction(int x, int y, unsigned int buttonMask) {
    this->updatePosition();
    this->_isDirty = false;
}

std::shared_ptr<WebXMouseCursor> WebXRdpMouse::getCursor(uint32_t cursorId) {
    return nullptr;
}

void WebXRdpMouse::updatePosition() {
    // Get the mouse cursor position


    // Check if the position has changed
    // if (this->_state->getX() != root_x || this->_state->getY() != root_y) {
    //     this->_state->setPosition(root_x, root_y);
    //     this->_isDirty = true;
    // }
}

WebXMouseState * WebXRdpMouse::createDefaultMouseState() {
    return new WebXMouseState(nullptr);
}





