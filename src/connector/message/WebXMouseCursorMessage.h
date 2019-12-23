#ifndef WEBX_MOUSE_CURSOR_MESSAGE_H
#define WEBX_MOUSE_CURSOR_MESSAGE_H

#include <utility>
#include <memory>
#include "WebXMessage.h"
#include <input/cursor/WebXMouseCursorImage.h>

class WebXMouseCursorMessage : public WebXMessage {
public:
    WebXMouseCursorMessage(int x, int y, std::shared_ptr<WebXMouseCursorImage>  mouseCursorImage, const char * mouseCursorName) :
        WebXMessage(Type::MouseCursor),
        x(x),
        y(y),
        mouseCursorImage(mouseCursorImage),
        mouseCursorName(mouseCursorName) {

    }
    
    virtual ~WebXMouseCursorMessage() {
    }

    int x;
    int y;

    std::shared_ptr<WebXMouseCursorImage> mouseCursorImage;
    std::string mouseCursorName;

};


#endif /* WEBX_MOUSE_CURSOR_MESSAGE_H */