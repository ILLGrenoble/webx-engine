#ifndef WEBX_WINDOWS_INSTRUCTION_H
#define WEBX_WINDOWS_INSTRUCTION_H

#include "WebXInstruction.h"

/**
 * @class WebXWindowsInstruction
 * @brief Represents an instruction to request visible Window deatils from the engine.
 * 
 * This class inherits from WebXInstruction and is used to handle
 * requests for visible window details. It initializes the instruction
 * type to Type::Windows.
 */
class WebXWindowsInstruction : public WebXInstruction {

public:
    /**
     * @brief Constructor for WebXWindowsInstruction.
     * @param clientId The ID of the client sending the instruction.
     * @param id The unique ID of the instruction.
     */
    WebXWindowsInstruction(uint32_t clientId, uint32_t id) :
        WebXInstruction(Type::Windows, clientId, id) {
    }

    /**
     * @brief Destructor for WebXWindowsInstruction.
     */
    virtual ~WebXWindowsInstruction() {
    }
};

#endif //WEBX_WINDOWS_INSTRUCTION_H
