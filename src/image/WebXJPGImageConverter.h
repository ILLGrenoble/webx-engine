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

    virtual WebXImage * convert(XImage * image, float quality) const;
    virtual WebXImage * convert(unsigned char * data, int width, int height, int bytesPerLine, int imageDepth, float quality) const;

private:
    WebXDataBuffer * _convert(unsigned char * data, int width, int height, int bytesPerLine, float quality) const;
    WebXDataBuffer * _convertMono(unsigned char * data, int width, int height, float quality) const;
};


#endif /* WEBX_JPG_IMAGE_CONVERTER_H */