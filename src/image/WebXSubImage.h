#ifndef WEBX_SUB_IMAGE_H
#define WEBX_SUB_IMAGE_H

#include <memory>
#include <models/WebXRectangle.h>

class WebXImage;

class WebXSubImage {
public:
    WebXSubImage(const WebXRectangle & imageRectangle, std::shared_ptr<WebXImage> image) :
        imageRectangle(imageRectangle),
        image(image) {
    } 
    virtual ~WebXSubImage() {}

    WebXRectangle imageRectangle;
    std::shared_ptr<WebXImage> image;
};


#endif /* WEBX_SUB_IMAGE_H */