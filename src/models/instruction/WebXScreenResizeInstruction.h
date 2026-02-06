#ifndef WEBX_SCREEN_RESIZE_INSTRUCTION_H
#define WEBX_SCREEN_RESIZE_INSTRUCTION_H

#include "WebXInstruction.h"

/**
 * @class WebXScreenResizeInstruction
 * @brief An instruction to request a screen resize
 */
class WebXScreenResizeInstruction : public WebXInstruction {

public:
    /**
     * @brief Constructor for WebXScreenResizeInstruction.
     * @param clientId The ID of the client sending the instruction.
     * @param id The unique ID of the instruction.
     * @param width The requested screen width.
     * @param height The requested screen height.
     */
    WebXScreenResizeInstruction(uint32_t clientId, uint32_t id, uint32_t width, uint32_t height) :
        WebXInstruction(Type::ScreenResize, clientId, id),
        width(width),
        height(height) {
    }

    /**
     * @brief Destructor for WebXScreenResizeInstruction.
     */
    virtual ~WebXScreenResizeInstruction() {
    }

    const uint32_t width;
    const uint32_t height;
};

#endif //WEBX_SCREEN_RESIZE_INSTRUCTION_H
