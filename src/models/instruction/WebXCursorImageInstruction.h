#ifndef WEBX_CURSOR_INSTRUCTION_H
#define WEBX_CURSOR_INSTRUCTION_H

#include "WebXInstruction.h"

/**
 * @class WebXCursorImageInstruction
 * @brief Represents a cursor image-related instruction in the WebX engine.
 * 
 * This class is derived from WebXInstruction and is used to handle cursor image-related events.
 * It includes information about the cursor ID.
 */
class WebXCursorImageInstruction : public WebXInstruction {

public:
    /**
     * @brief Constructor for WebXCursorImageInstruction.
     * @param clientId The ID of the client sending the instruction.
     * @param id The unique ID of the instruction.
     * @param cursorId The ID of the cursor image.
     */
    WebXCursorImageInstruction(uint32_t clientId, uint32_t id, uint32_t cursorId) :
        WebXInstruction(Type::Cursor, clientId, id),
        cursorId(cursorId) {
    }
    
    /**
     * @brief Destructor for WebXCursorImageInstruction.
     */
    virtual ~WebXCursorImageInstruction() {
    }

    uint32_t cursorId;
};

#endif //WEBX_CURSOR_INSTRUCTION_H
