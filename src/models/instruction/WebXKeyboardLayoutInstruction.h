#ifndef WEBX_KEYBOARD_LAYOUT_INSTRUCTION_H
#define WEBX_KEYBOARD_LAYOUT_INSTRUCTION_H

#include <memory>
#include "WebXInstruction.h"

class WebXKeyboardLayoutInstruction : public WebXInstruction {
public:
    WebXKeyboardLayoutInstruction(uint32_t clientId, uint32_t id, const std::string & keyboardLayout) :
        WebXInstruction(Type::KeyboardLayout, clientId, id),
        keyboardLayout(keyboardLayout) {}
    virtual ~WebXKeyboardLayoutInstruction() {}

    const std::string keyboardLayout;
};

#endif /* WEBX_KEYBOARD_LAYOUT_INSTRUCTION_H */