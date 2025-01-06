#ifndef WEBX_MESSAGE_H
#define WEBX_MESSAGE_H

#include <string>
#include <cstdint>

class WebXMessage {
public:
    enum Type {
        Windows = 2,
        Image,
        Screen,
        Subimages,
        Mouse,
        CursorImage,
        Void
    };

    WebXMessage(Type type) :
        type(type),
        commandId(-1) {}
    virtual ~WebXMessage() {}

    Type type;
    uint32_t commandId;
};


#endif /* WEBX_MESSAGE_H */