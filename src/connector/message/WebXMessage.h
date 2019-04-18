#ifndef WEBX_MESSAGE_H
#define WEBX_MESSAGE_H

#include <string>

class WebXMessage {
public:
    WebXMessage(const std::string & type) :
        type(type) {}
    virtual ~WebXMessage() {}

    std::string type;
};


#endif /* WEBX_MESSAGE_H */