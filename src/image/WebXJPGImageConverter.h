#ifndef WEBX_JPG_IMAGE_CONVERTER_H
#define WEBX_JPG_IMAGE_CONVERTER_H

#include "WebXImageConverter.h"
#include <stdlib.h>

class WebXImage;
class WebXDataBuffer;

class WebXJPGImageConverter : public WebXImageConverter {
public:
    WebXJPGImageConverter();
    virtual ~WebXJPGImageConverter();

    virtual WebXImage * convert(XImage * image, bool hasAlphaChannel) const;
    virtual WebXImage * convert(unsigned char * data, int width, int height, int bytesPerLine, int imageDepth) const;

private:
    WebXDataBuffer * _convert(unsigned char * data, int width, int height, int bytesPerLine, int imageDepth) const;
};


#endif /* WEBX_JPG_IMAGE_CONVERTER_H */