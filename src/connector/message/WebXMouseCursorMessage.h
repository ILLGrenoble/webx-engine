#ifndef WEBX_MOUSE_CURSOR_MESSAGE_H
#define WEBX_MOUSE_CURSOR_MESSAGE_H

#include <utility>
#include <memory>
#include "WebXMessage.h"
#include <image/WebXImage.h>

class WebXMouseCursorMessage : public WebXMessage {
public:
    WebXMouseCursorMessage(int x, int y, int xhot, int yhot, std::shared_ptr<WebXImage> mouseCursorImage, const std::string & mouseCursorName) :
        WebXMessage(Type::MouseCursor),
        x(x),
        y(y),
        xhot(xhot),
        yhot(yhot),
        mouseCursorImage(mouseCursorImage),
        mouseCursorName(mouseCursorName) {

    }
    
    virtual ~WebXMouseCursorMessage() {
    }

    int x;
    int y;
    int xhot;
    int yhot;

    std::shared_ptr<WebXImage> mouseCursorImage;
    std::string mouseCursorName;

};


#endif /* WEBX_MOUSE_CURSOR_MESSAGE_H */