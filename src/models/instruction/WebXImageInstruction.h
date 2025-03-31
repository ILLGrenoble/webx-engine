#ifndef WEBX_IMAGE_INSTRUCTION_H
#define WEBX_IMAGE_INSTRUCTION_H

#include "WebXInstruction.h"

/**
 * @class WebXImageInstruction
 * @brief Represents an instruction to retrieve a window image from the WebX engine.
 * 
 * This class is derived from WebXInstruction and is used to handle window image requests.
 * It includes information about the associated window ID.
 */
class WebXImageInstruction : public WebXInstruction {

public:
    /**
     * @brief Constructor for WebXImageInstruction.
     * @param clientId The ID of the client sending the instruction.
     * @param id The unique ID of the instruction.
     * @param windowId The ID of the window associated with the image.
     */
    WebXImageInstruction(uint32_t clientId, uint32_t id, uint32_t windowId) :
        WebXInstruction(Type::Image, clientId, id),
        windowId(windowId) {
    }

    /**
     * @brief Destructor for WebXImageInstruction.
     */
    virtual ~WebXImageInstruction() {
    }

    const uint32_t windowId;
};

#endif //WEBX_IMAGE_INSTRUCTION_H
