#ifndef WEBX_DISCONNECT_MESSAGE_H
#define WEBX_DISCONNECT_MESSAGE_H

#include "WebXMessage.h"

class WebXDisconnectMessage : public WebXMessage {
public:
    WebXDisconnectMessage(uint64_t clientIndexMask) :
        WebXMessage(Type::Disconnect, clientIndexMask) {}
    
    virtual ~WebXDisconnectMessage() {}
};


#endif /* WEBX_DISCONNECT_MESSAGE_H */