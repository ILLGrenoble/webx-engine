#ifndef WEBX_IMAGE_MESSAGE_H
#define WEBX_IMAGE_MESSAGE_H

#include <vector>
#include <memory>
#include "WebXMessage.h"

class WebXImage;

class WebXImageMessage : public WebXMessage {
public:
    WebXImageMessage(std::shared_ptr<WebXImage> image) :
        _image(image) {}
    virtual ~WebXImageMessage() {}

    virtual void toJson(nlohmann::json & j) const;

private:
    std::shared_ptr<WebXImage> _image;
};


#endif /* WEBX_IMAGE_MESSAGE_H*/