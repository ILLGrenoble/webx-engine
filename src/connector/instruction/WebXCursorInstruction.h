#ifndef WEBX_CURSOR_INSTRUCTION_H
#define WEBX_CURSOR_INSTRUCTION_H

#include "WebXInstruction.h"

class WebXCursorInstruction : public WebXInstruction {

public:
    WebXCursorInstruction(uint32_t id) :
        WebXInstruction(Type::Cursor, id) {
    }

    virtual ~WebXCursorInstruction() {
    }
};

#endif //WEBX_CURSOR_INSTRUCTION_H
