#ifndef WEBX_CURSOR_IMAGE_MESSAGE_H
#define WEBX_CURSOR_IMAGE_MESSAGE_H

#include <utility>
#include <memory>
#include "WebXMessage.h"
#include <image/WebXImage.h>

class WebXCursorImageMessage : public WebXMessage {
public:
    WebXCursorImageMessage(int xhot, int yhot, uint32_t cursorId, std::shared_ptr<WebXImage> mouseCursorImage) :
        WebXMessage(Type::CursorImage),
        xhot(xhot),
        yhot(yhot),
        cursorId(cursorId),
        mouseCursorImage(mouseCursorImage) {
    }
    
    virtual ~WebXCursorImageMessage() {
    }

    int xhot;
    int yhot;
    uint32_t cursorId;
    std::shared_ptr<WebXImage> mouseCursorImage;
};


#endif /* WEBE_CURSOR_IMAGE_MESSAGE_H */