#ifndef WEBX_JPG_IMAGE_CONVERTER_H
#define WEBX_JPG_IMAGE_CONVERTER_H

#include "WebXImageConverter.h"
#include <stdlib.h>

class WebXImage;

class WebXJPGImageConverter : public WebXImageConverter{
public:
    WebXJPGImageConverter();
    virtual ~WebXJPGImageConverter();

    virtual WebXImage * convert(XImage * image, bool hasAlphaChannel) const;

};


#endif /* WEBX_JPG_IMAGE_CONVERTER_H */