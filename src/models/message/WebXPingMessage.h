#ifndef WEBX_PING_MESSAGE_H
#define WEBX_PING_MESSAGE_H

#include "WebXMessage.h"

class WebXPingMessage : public WebXMessage {
public:
    WebXPingMessage(uint64_t clientIndexMask) :
        WebXMessage(Type::Ping, clientIndexMask) {}
    virtual ~WebXPingMessage() {}
};


#endif /* WEBX_PING_MESSAGE_H */