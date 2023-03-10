#ifndef WEBX_SCREEN_INSTRUCTION_H
#define WEBX_SCREEN_INSTRUCTION_H

#include "WebXInstruction.h"

class WebXScreenInstruction : public WebXInstruction {

public:
    WebXScreenInstruction(uint32_t id) :
        WebXInstruction(Type::Screen, id) {
    }

    virtual ~WebXScreenInstruction() {
    }
};

#endif //WEBX_SCREEN_INSTRUCTION_H
