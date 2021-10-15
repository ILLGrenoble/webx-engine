#ifndef WEBX_PNG_IMAGE_CONVERTER_H
#define WEBX_PNG_IMAGE_CONVERTER_H

#include "WebXImageConverter.h"
#include <png.h>
#include <stdlib.h>

class WebXImage;

class WebXPNGImageConverter : public WebXImageConverter {
public:
    WebXPNGImageConverter();
    virtual ~WebXPNGImageConverter();

    virtual WebXImage * convert(XImage * image, bool hasAlphaChannel) const;
    virtual WebXImage * convert(unsigned char * data, int width, int height, int bytesPerLine, int imageDepth) const;

private:
    static void RawDataWriter(png_struct * png, png_byte * data, png_size_t length);
    static unsigned long upperPowerOfTwo(unsigned long v);

private:
    class RawData {
    public:
        RawData() :
            buffer(0),
            capacity(0),
            size(0) {}
        RawData(size_t initialCapacity) :
            buffer(0),
            capacity(initialCapacity),
            size(0) {
            buffer = (unsigned char *)malloc(initialCapacity);
        }
        virtual ~RawData() {}

        unsigned char * buffer;
        size_t capacity;
        size_t size;
    };

};


#endif /* WEBX_PNG_IMAGE_CONVERTER_H */