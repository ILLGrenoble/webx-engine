#ifndef WEBX_SUB_IMAGES_MESSAGE_H
#define WEBX_SUB_IMAGES_MESSAGE_H

#include <vector>
#include "WebXMessage.h"
#include <image/WebXSubImage.h>

class WebXSubImagesMessage : public WebXMessage {
public:
    WebXSubImagesMessage(uint64_t clientIndexMask, uint32_t windowId, const std::vector<WebXSubImage> & images) :
        WebXMessage(Type::Subimages, clientIndexMask),
        windowId(windowId),
        images(images) {}
    virtual ~WebXSubImagesMessage() {}

    uint32_t windowId;
    std::vector<WebXSubImage> images;
};


#endif /* WEBX_SUB_IMAGES_MESSAGE_H*/