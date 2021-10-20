#ifndef WEBX_WEBP_IMAGE_CONVERTER_H
#define WEBX_WEBP_IMAGE_CONVERTER_H

#include "WebXImageConverter.h"
#include <stdlib.h>

class WebXImage;

class WebXWebPImageConverter : public WebXImageConverter {
public:
    WebXWebPImageConverter();
    virtual ~WebXWebPImageConverter();

    virtual WebXImage * convert(XImage * image) const;
    virtual WebXImage * convert(unsigned char * data, int width, int height, int bytesPerLine, int imageDepth) const;

};


#endif /* WEBX_WEBP_IMAGE_CONVERTER_H */