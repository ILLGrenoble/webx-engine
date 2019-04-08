#ifndef WEBX_IMAGE_MESSAGE_H
#define WEBX_IMAGE_MESSAGE_H

#include <vector>
#include <memory>
#include "WebXMessage.h"

class WebXImage;

class WebXImageMessage : public WebXMessage {
public:
    WebXImageMessage(unsigned long windowId, std::shared_ptr<WebXImage> image) :
        _windowId(windowId),
        _image(image) {}
    virtual ~WebXImageMessage() {}

    virtual void toJson(nlohmann::json & j) const;

private:
    unsigned long _windowId;
    std::shared_ptr<WebXImage> _image;
};


#endif /* WEBX_IMAGE_MESSAGE_H*/