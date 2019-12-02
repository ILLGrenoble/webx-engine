#ifndef WEBX_MOUSE_CURSOR_MESSAGE_H
#define WEBX_MOUSE_CURSOR_MESSAGE_H

#include <utility>

#include "WebXMessage.h"

#include "input/cursor/WebXMouseCursor.h"

class WebXMouseCursorMessage : public WebXMessage {
public:
    WebXMouseCursorMessage(int x, int y, WebXMouseCursor *  mouseCursor) :
        WebXMessage(Type::MouseCursor),
        x(x),
        y(y),
        mouseCursor(mouseCursor){

    }
    
    virtual ~WebXMouseCursorMessage() {}

    int x;
    int y;

    WebXMouseCursor * mouseCursor;

};


#endif /* WEBX_MOUSE_CURSOR_MESSAGE_H */