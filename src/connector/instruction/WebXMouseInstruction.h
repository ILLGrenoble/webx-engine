#ifndef WEBX_MOUSE_INSTRUCTION_H
#define WEBX_MOUSE_INSTRUCTION_H

#include "WebXInstruction.h"

class WebXMouseInstruction : public WebXInstruction {

public:

    WebXMouseInstruction(uint32_t id, int x, int y, unsigned int buttonMask) :
            WebXInstruction(Type::Mouse, id),
            x(x),
            y(y),
            buttonMask(buttonMask){
    }

    virtual ~WebXMouseInstruction() {
    }

    int x;
    int y;
    unsigned int buttonMask;

};

#endif //WEBX_MOUSE_INSTRUCTION_H
