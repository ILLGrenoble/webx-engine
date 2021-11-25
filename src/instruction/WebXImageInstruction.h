#ifndef WEBX_IMAGE_INSTRUCTION_H
#define WEBX_IMAGE_INSTRUCTION_H

#include "WebXInstruction.h"

class WebXImageInstruction : public WebXInstruction {

public:
    WebXImageInstruction(uint32_t id, unsigned long windowId) :
        WebXInstruction(Type::Image, id),
        windowId(windowId) {
    }

    virtual ~WebXImageInstruction() {
    }

    uint32_t windowId;
};


#endif //WEBX_IMAGE_INSTRUCTION_H
