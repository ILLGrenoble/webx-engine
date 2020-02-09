#ifndef WEBX_MOUSE_INSTRUCTION_H
#define WEBX_MOUSE_INSTRUCTION_H

#include "WebXInstruction.h"

class WebXMouseInstruction : public WebXInstruction {

public:
    WebXMouseInstruction(uint32_t id, int x, int y, unsigned int buttonMask) :
        WebXInstruction(Type::Mouse, id),
        x(x),
        y(y),
        buttonMask(buttonMask) {
    }

    WebXMouseInstruction() :
        WebXInstruction(Type::Mouse, id),
        x(0),
        y(0),
        buttonMask(0) {
    }

    virtual ~WebXMouseInstruction() {
    }

    void set(uint32_t id, int x, int y, unsigned int buttonMask) {
        this->id = id;
        this->x = x;
        this->y = y;
        this->buttonMask = buttonMask;
    }

    int x;
    int y;
    unsigned int buttonMask;

};

#endif //WEBX_MOUSE_INSTRUCTION_H
