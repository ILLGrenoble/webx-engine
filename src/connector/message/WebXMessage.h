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
        Subimages
    };

    WebXMessage(Type type) :
        type(type),
        commandId(-1) {}
    virtual ~WebXMessage() {}

    Type type;
    long commandId;
};


#endif /* WEBX_MESSAGE_H */