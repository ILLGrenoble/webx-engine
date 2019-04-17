#ifndef WEBX_COMBINED_IMAGE_CONVERTER_H
#define WEBX_COMBINED_IMAGE_CONVERTER_H

#include "WebXImageConverter.h"
class WebXPNGImageConverter;
class WebXJPGImageConverter;

class WebXCombinedImageConverter : public WebXImageConverter {
public:
    WebXCombinedImageConverter();
    virtual ~WebXCombinedImageConverter();

    virtual WebXImage * convert(XImage * image, bool hasAlphaChannel) const;

private:
    WebXPNGImageConverter * _pngImageConverter;
    WebXJPGImageConverter * _jpgImageConverter;
};


#endif /* WEBX_COMBINED_IMAGE_CONVERTER_H */