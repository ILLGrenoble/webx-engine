#ifndef WEBX_CONNECTION_H
#define WEBX_CONNECTION_H

#include <vector>
#include <memory>
#include <image/WebXSubImage.h>
#include "WebXWindowProperties.h"

class WebXConnection {
public:
    WebXConnection() {}
    virtual ~WebXConnection() {}

    virtual void onDisplayChanged(const std::vector<WebXWindowProperties> & windows) = 0;
    virtual void onImageChanged(unsigned long windowId, std::shared_ptr<WebXImage> image) = 0;
    virtual void onSubImagesChanged(unsigned long windowId, std::vector<WebXSubImage> subImages) = 0;
    virtual void onMouseChanged(int x, int y, uint32_t cursorId) = 0;
};


#endif /* WEBX_CONNECTION_H */