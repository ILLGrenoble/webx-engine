#ifndef WEBX_CONNECT_INSTRUCTION_H
#define WEBX_CONNECT_INSTRUCTION_H

#include "WebXInstruction.h"

class WebXConnectInstruction : public WebXInstruction {

public:
    WebXConnectInstruction(unsigned long id) :
        WebXInstruction(Type::Connect, id) {
    }

    virtual ~WebXConnectInstruction() {
    }
};

#endif //WEBX_CONNECT_INSTRUCTION_H
