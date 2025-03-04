#ifndef WEBX_PONG_INSTRUCTION_H
#define WEBX_PONG_INSTRUCTION_H

#include "WebXInstruction.h"

class WebXPongInstruction : public WebXInstruction {

public:
    WebXPongInstruction(uint32_t clientId, uint32_t id) :
        WebXInstruction(Type::Pong, clientId, id) {
    }

    virtual ~WebXPongInstruction() {
    }
};


#endif //WEBX_PONG_INSTRUCTION_H
