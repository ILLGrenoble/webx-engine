#ifndef WEBX_PNG_IMAGE_CONVERTER_H
#define WEBX_PNG_IMAGE_CONVERTER_H

#include "WebXImageConverter.h"
#include <png.h>

class WebXImage;

class WebXPNGImageConverter : public WebXImageConverter{

public:
    WebXPNGImageConverter();
    virtual ~WebXPNGImageConverter();

    virtual WebXImage * convert(XImage * image, WebXRectangle * subWindowRectangle) const;

private:
    static void RawDataWriter(png_struct * png, png_byte * data, png_size_t length);

private:
    class RawData {
    public:
        RawData() :
            buffer(0),
            size(0) {}
        virtual ~RawData() {}

        char * buffer;
        size_t size;
    };

};


#endif /* WEBX_PNG_IMAGE_CONVERTER_H */