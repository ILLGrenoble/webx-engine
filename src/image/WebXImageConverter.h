#ifndef WEBX_IMAGE_CONVERTER_H
#define WEBX_IMAGE_CONVERTER_H

#include <X11/Xlib.h>
#include <spdlog/spdlog.h>

class WebXImage;

class WebXImageConverter {
public:
    WebXImageConverter() {}
    virtual ~WebXImageConverter() {}

    virtual WebXImage * convert(XImage * image, float quality) const = 0;
    virtual WebXImage * convert(unsigned char * data, int width, int height, int bytesPerLine, int imageDepth, float quality) const = 0;
};


#endif /* WEBX_IMAGE_CONVERTER_H */