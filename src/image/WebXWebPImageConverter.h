#ifndef WEBX_WEBP_IMAGE_CONVERTER_H
#define WEBX_WEBP_IMAGE_CONVERTER_H

#include "WebXImageConverter.h"
#include <stdlib.h>

class WebXImage;

class WebXWebPImageConverter : public WebXImageConverter{
public:
    WebXWebPImageConverter();
    virtual ~WebXWebPImageConverter();

    virtual WebXImage * convert(XImage * image, bool hasAlphaChannel) const;

};


#endif /* WEBX_WEBP_IMAGE_CONVERTER_H */