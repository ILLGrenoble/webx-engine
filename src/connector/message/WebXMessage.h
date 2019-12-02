#ifndef WEBX_MESSAGE_H
#define WEBX_MESSAGE_H

#include <string>

class WebXMessage {
public:
    enum Type {
        Connection = 1,
        Windows,
        Image,
        Screen,
        Subimages,
        MouseCursor
    };

    WebXMessage(Type type) :
        type(type),
        commandId(-1) {}
    virtual ~WebXMessage() {}

    Type type;
    uint32_t commandId;
};


#endif /* WEBX_MESSAGE_H */