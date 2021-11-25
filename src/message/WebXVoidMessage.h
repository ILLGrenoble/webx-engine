#ifndef WEBX_VOID_MESSAGE_H
#define WEBX_VOID_MESSAGE_H

#include "WebXMessage.h"

class WebXVoidMessage : public WebXMessage {
public:
    WebXVoidMessage() :
        WebXMessage(Type::Void) {}
    
    virtual ~WebXVoidMessage() {}
};


#endif /* WEBX_VOID_MESSAGE_H */