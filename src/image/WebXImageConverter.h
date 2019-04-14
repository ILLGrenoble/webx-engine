#ifndef WEBX_IMAGE_CONVERTER_H
#define WEBX_IMAGE_CONVERTER_H

#include <X11/Xlib.h>

class WebXImage;

class WebXImageConverter {
public:
    WebXImageConverter() {}
    virtual ~WebXImageConverter() {}

    virtual WebXImage * convert(XImage * image, bool hasAlphaChannel) const = 0;

private:

};


#endif /* WEBX_IMAGE_CONVERTER_H */