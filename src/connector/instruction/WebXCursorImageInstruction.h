#ifndef WEBX_CURSOR_INSTRUCTION_H
#define WEBX_CURSOR_INSTRUCTION_H

#include "WebXInstruction.h"

class WebXCursorImageInstruction : public WebXInstruction {

public:
    WebXCursorImageInstruction(uint32_t id, uint32_t cursorId) :
        WebXInstruction(Type::Cursor, id),
        cursorId(cursorId) {
    }
    virtual ~WebXCursorImageInstruction() {
    }

    uint32_t cursorId;
};

#endif //WEBX_CURSOR_INSTRUCTION_H
