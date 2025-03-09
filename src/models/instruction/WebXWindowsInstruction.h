#ifndef WEBX_WINDOWS_INSTRUCTION_H
#define WEBX_WINDOWS_INSTRUCTION_H

#include "WebXInstruction.h"

class WebXWindowsInstruction : public WebXInstruction {

public:
    WebXWindowsInstruction(uint32_t clientId, uint32_t id) :
        WebXInstruction(Type::Windows, clientId, id) {
    }

    virtual ~WebXWindowsInstruction() {
    }
};

#endif //WEBX_WINDOWS_INSTRUCTION_H
