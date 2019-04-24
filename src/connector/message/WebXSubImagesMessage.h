#ifndef WEBX_SUB_IMAGES_MESSAGE_H
#define WEBX_SUB_IMAGES_MESSAGE_H

#include <vector>
#include "WebXMessage.h"
#include <image/WebXSubImage.h>

class WebXSubImagesMessage : public WebXMessage {
public:
    WebXSubImagesMessage(unsigned long windowId, std::vector<WebXSubImage> images) :
        WebXMessage(Type::Subimages),
        windowId(windowId),
        images(images) {}
    virtual ~WebXSubImagesMessage() {}

    unsigned long windowId;
    std::vector<WebXSubImage> images;
};


#endif /* WEBX_SUB_IMAGES_MESSAGE_H*/