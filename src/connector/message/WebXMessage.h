#ifndef WEBX_MESSAGE_H
#define WEBX_MESSAGE_H

#include <string>

class WebXMessage {
public:
    WebXMessage(const std::string & type) :
        type(type),
        commandId(-1) {}
    virtual ~WebXMessage() {}

    std::string type;
    long commandId;
};


#endif /* WEBX_MESSAGE_H */