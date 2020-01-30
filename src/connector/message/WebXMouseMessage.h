#ifndef WEBX_MOUSE_MESSAGE_H
#define WEBX_MOUSE_MESSAGE_H

#include "WebXMessage.h"

class WebXMouseMessage : public WebXMessage {
public:
    WebXMouseMessage(int x, int y, uint32_t cursorId) :
        WebXMessage(Type::Mouse),
        x(x),
        y(y),
        cursorId(cursorId) {
    }
    
    virtual ~WebXMouseMessage() {
    }

    int x;
    int y;
    uint32_t cursorId;
};


#endif /* WEBX_MOUSE_MESSAGE_H */