#ifndef WEBX_COMBINED_IMAGE_CONVERTER_H
#define WEBX_COMBINED_IMAGE_CONVERTER_H

#include "WebXImageConverter.h"
#include "WebXPNGImageConverter.h"
#include "WebXJPGImageConverter.h"

class WebXCombinedImageConverter : public WebXImageConverter {
public:
    WebXCombinedImageConverter();
    virtual ~WebXCombinedImageConverter();

    virtual WebXImage * convert(XImage * image, bool hasAlphaChannel) const;
    virtual WebXImage * convert(const unsigned char * data, int width, int height, int bytesPerLine, int imageDepth) const;

private:
    WebXPNGImageConverter _pngImageConverter;
    WebXJPGImageConverter _jpgImageConverter;
};


#endif /* WEBX_COMBINED_IMAGE_CONVERTER_H */