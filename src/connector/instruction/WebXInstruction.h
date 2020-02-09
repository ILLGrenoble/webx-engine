#ifndef WEBX_INSTRUCTION_H
#define WEBX_INSTRUCTION_H

#include <string>

class WebXInstruction {
public:
    enum Type {
        Connect = 1,
        Windows,
        Image,
        Screen,
        Mouse,
        Keyboard,
        Cursor
    };

    WebXInstruction(Type type, uint32_t id) :
        type(type),
        id(id) {
    }

    WebXInstruction(Type type) :
        type(type),
        id(0) {
    }

    virtual ~WebXInstruction() {}

    Type type;
    uint32_t id;

};


#endif /* WEBX_INSTRUCTION_H */