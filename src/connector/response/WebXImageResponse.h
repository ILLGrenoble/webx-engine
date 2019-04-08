#ifndef WEBX_IMAGE_RESPONSE_H
#define WEBX_IMAGE_RESPONSE_H

#include <vector>
#include <memory>
#include "WebXResponse.h"

class WebXImage;

class WebXImageResponse : public WebXResponse {
public:
    WebXImageResponse(unsigned long windowId, std::shared_ptr<WebXImage> image) :
        _windowId(windowId),
        _image(image) {}
    virtual ~WebXImageResponse() {}

    virtual void toJson(nlohmann::json & j) const;

private:
    unsigned long _windowId;
    std::shared_ptr<WebXImage> _image;
};


#endif /* WEBX_IMAGE_RESPONSE_H*/