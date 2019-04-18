#ifndef WEBX_INSTRUCTION_H
#define WEBX_INSTRUCTION_H

#include <string>

class WebXInstruction {
public:
    enum Type {
        Connect = 1,
        Image,
        Images,
        Window,
        Windows,
        Mouse
    };

    WebXInstruction() {}
    virtual ~WebXInstruction() {}

    Type type;
    std::string stringPayload;
    long numericPayload;
};


#endif /* WEBX_INSTRUCTION_H */