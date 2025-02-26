#ifndef WEBX_INSTRUCTION_H
#define WEBX_INSTRUCTION_H

#include <string>

class WebXInstruction {
public:
    enum Type {
        Windows = 2,
        Image,
        Screen,
        Mouse,
        Keyboard,
        Cursor,
        Quality
    };

    WebXInstruction(Type type, uint32_t clientId, uint32_t id) :
        type(type),
        id(id),
        clientId(clientId) {
    }

    virtual ~WebXInstruction() {}

    Type type;
    uint32_t clientId;
    uint32_t id;

};


#endif /* WEBX_INSTRUCTION_H */