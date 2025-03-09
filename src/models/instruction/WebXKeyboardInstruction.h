#ifndef WEBX_KEYBOARD_INSTRUCTION_H
#define WEBX_KEYBOARD_INSTRUCTION_H

#include "WebXInstruction.h"

class WebXKeyboardInstruction : public WebXInstruction {

public:
    WebXKeyboardInstruction(uint32_t clientId, uint32_t id, int key, bool pressed) :
        WebXInstruction(Type::Keyboard, clientId, id),
        key(key),
        pressed(pressed) {
    }

    virtual ~WebXKeyboardInstruction() {
    }

    int key;
    bool pressed;

};

#endif //WEBX_KEYBOARD_INSTRUCTION_H
