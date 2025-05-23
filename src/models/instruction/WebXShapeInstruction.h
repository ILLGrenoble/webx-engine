#ifndef WEBX_SHAPE_INSTRUCTION_H
#define WEBX_SHAPE_INSTRUCTION_H

#include "WebXInstruction.h"

/**
 * @class WebXShapeInstruction
 * @brief Represents an instruction to retrieve a window shape from the WebX engine (stencil map image).
 * 
 * This class is derived from WebXInstruction and is used to handle window shape requests.
 * It includes information about the associated window ID.
 */
class WebXShapeInstruction : public WebXInstruction {

public:
    /**
     * @brief Constructor for WebXShapeInstruction.
     * @param clientId The ID of the client sending the instruction.
     * @param id The unique ID of the instruction.
     * @param windowId The ID of the window associated with the image.
     */
    WebXShapeInstruction(uint32_t clientId, uint32_t id, uint32_t windowId) :
        WebXInstruction(Type::Shape, clientId, id),
        windowId(windowId) {
    }

    /**
     * @brief Destructor for WebXShapeInstruction.
     */
    virtual ~WebXShapeInstruction() {
    }

    const uint32_t windowId;
};

#endif //WEBX_SHAPE_INSTRUCTION_H
