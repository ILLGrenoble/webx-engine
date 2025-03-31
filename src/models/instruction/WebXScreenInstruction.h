#ifndef WEBX_SCREEN_INSTRUCTION_H
#define WEBX_SCREEN_INSTRUCTION_H

#include "WebXInstruction.h"

/**
 * @class WebXScreenInstruction
 * @brief Represents an instruction specific to requesting screen details (ie size).
 * 
 * This class inherits from WebXInstruction and is used to handle
 * requests for screen details. It initializes the
 * instruction type to Type::Screen.
 */
class WebXScreenInstruction : public WebXInstruction {

public:
    /**
     * @brief Constructor for WebXScreenInstruction.
     * @param clientId The ID of the client sending the instruction.
     * @param id The unique ID of the instruction.
     */
    WebXScreenInstruction(uint32_t clientId, uint32_t id) :
        WebXInstruction(Type::Screen, clientId, id) {
    }

    /**
     * @brief Destructor for WebXScreenInstruction.
     */
    virtual ~WebXScreenInstruction() {
    }
};

#endif //WEBX_SCREEN_INSTRUCTION_H
