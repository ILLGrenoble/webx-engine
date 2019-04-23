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
        Mouse
    };

    WebXInstruction(Type type, unsigned long id) :
        type(type),
        id(id) {}

    virtual ~WebXInstruction() {}

    Type type;

    long id;

};


#endif /* WEBX_INSTRUCTION_H */