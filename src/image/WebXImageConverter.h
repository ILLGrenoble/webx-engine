#ifndef WEBX_IMAGE_CONVERTER_H
#define WEBX_IMAGE_CONVERTER_H

#include <X11/Xlib.h>
#include <spdlog/spdlog.h>

class WebXImage;

class WebXImageConverter {
public:
    WebXImageConverter() {}
    virtual ~WebXImageConverter() {}

    virtual WebXImage * convert(XImage * image) const = 0;
    virtual WebXImage * convert(unsigned char * data, int width, int height, int bytesPerLine, int imageDepth) const = 0;

    void setQuality(float quality) {
        if (quality > 0.0 && quality <= 1.0) {
            this->_quality = quality;
        
        } else {
            spdlog::warn("Invalid quality {} sent to WebXImageConverter", quality);
        }
    }

protected:
    float _quality = 0.9;
};


#endif /* WEBX_IMAGE_CONVERTER_H */