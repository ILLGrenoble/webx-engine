#ifndef WEBX_IMAGE_MESSAGE_H
#define WEBX_IMAGE_MESSAGE_H

#include <vector>
#include <memory>
#include "WebXMessage.h"

class WebXImage;

class WebXImageMessage : public WebXMessage {
public:
    WebXImageMessage(uint32_t windowId, std::shared_ptr<WebXImage> image) :
        WebXMessage(Type::Image),
        windowId(windowId),
        image(image) {}
    virtual ~WebXImageMessage() {}

    uint32_t windowId;
    std::shared_ptr<WebXImage> image;
};


#endif /* WEBX_IMAGE_MESSAGE_H*/