#ifndef WEBX_SCREEN_MESSAGE_H
#define WEBX_SCREEN_MESSAGE_H

#include "WebXMessage.h"
#include <utils/WebXSize.h>
#include <string>

class WebXScreenMessage : public WebXMessage {
public:
    WebXScreenMessage(WebXSize screenSize) :
        WebXMessage(Type::Screen),
        screenSize(screenSize) {}
    
    virtual ~WebXScreenMessage() {}

    WebXSize screenSize;
};


#endif /* WEBX_SCREEN_MESSAGE_H */