#ifndef WEBX_KEYBOARD_INSTRUCTION_H
#define WEBX_KEYBOARD_INSTRUCTION_H

#include "WebXInstruction.h"

/**
 * @class WebXKeyboardInstruction
 * @brief Represents a keyboard-related instruction in the WebX engine.
 * 
 * This class is derived from WebXInstruction and is used to handle keyboard events.
 * It includes information about the key pressed and its state.
 */
class WebXKeyboardInstruction : public WebXInstruction {

public:
    /**
     * @brief Constructor for WebXKeyboardInstruction.
     * @param clientId The ID of the client sending the instruction.
     * @param id The unique ID of the instruction.
     * @param key The key code of the keyboard event.
     * @param pressed The state of the key (pressed or released).
     */
    WebXKeyboardInstruction(uint32_t clientId, uint32_t id, int key, bool pressed) :
        WebXInstruction(Type::Keyboard, clientId, id),
        key(key),
        pressed(pressed) {
    }

    /**
     * @brief Destructor for WebXKeyboardInstruction.
     */
    virtual ~WebXKeyboardInstruction() {
    }

    const int key;
    const bool pressed;

};

#endif //WEBX_KEYBOARD_INSTRUCTION_H
