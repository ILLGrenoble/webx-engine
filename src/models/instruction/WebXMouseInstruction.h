#ifndef WEBX_MOUSE_INSTRUCTION_H
#define WEBX_MOUSE_INSTRUCTION_H

#include "WebXInstruction.h"

/**
 * @class WebXMouseInstruction
 * @brief Represents a mouse-related instruction in the WebX engine.
 * 
 * This class is derived from WebXInstruction and is used to handle mouse events.
 * It includes information about the mouse position and button states.
 */
class WebXMouseInstruction : public WebXInstruction {

public:
    /**
     * @brief Constructor for WebXMouseInstruction.
     * @param clientId The ID of the client sending the instruction.
     * @param id The unique ID of the instruction.
     * @param x The x-coordinate of the mouse.
     * @param y The y-coordinate of the mouse.
     * @param buttonMask The state of mouse buttons.
     */
    WebXMouseInstruction(uint32_t clientId, uint32_t id, int x, int y, unsigned int buttonMask) :
        WebXInstruction(Type::Mouse, clientId, id),
        x(x),
        y(y),
        buttonMask(buttonMask) {
    }

    /**
     * @brief Destructor for WebXMouseInstruction.
     */
    virtual ~WebXMouseInstruction() {
    }

    const int x;
    const int y;
    const unsigned int buttonMask;

};

#endif //WEBX_MOUSE_INSTRUCTION_H
