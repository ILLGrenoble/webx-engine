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
        Ping,
    };

    WebXMessage(Type type, uint64_t clientIndexMask) :
        type(type),
        clientIndexMask(clientIndexMask),
        commandId(-1) {}
    virtual ~WebXMessage() {}

    Type type;
    uint64_t clientIndexMask;
    uint32_t commandId;
};


#endif /* WEBX_MESSAGE_H */