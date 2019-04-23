#ifndef WEBX_INSTRUCTION_H
#define WEBX_INSTRUCTION_H

#include <string>

class WebXInstruction {
public:
    enum Type {
        Connect = 1,
        Windows,
        Image,
        Screen
    };

    WebXInstruction() :
     id(-1) {}
    virtual ~WebXInstruction() {}

    Type type;
    long id;
    std::string stringPayload;
    long numericPayload;
};


#endif /* WEBX_INSTRUCTION_H */