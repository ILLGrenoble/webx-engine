#ifndef WEBX_JPG_IMAGE_CONVERTER_H
#define WEBX_JPG_IMAGE_CONVERTER_H

#include "WebXImageConverter.h"
#include <stdlib.h>

class WebXImage;

class WebXJPGImageConverter : public WebXImageConverter{
private:
    class RawData {
    public:
        RawData() :
            buffer(0),
            capacity(128),
            size(0) {
            buffer = (unsigned char *)malloc(capacity);
        }
        RawData(size_t initialCapacity) :
            buffer(0),
            capacity(initialCapacity),
            size(0) {
            buffer = (unsigned char *)malloc(capacity);
        }
        virtual ~RawData() {}

        unsigned char * buffer;
        size_t capacity;
        size_t size;
    };

public:
    WebXJPGImageConverter();
    virtual ~WebXJPGImageConverter();

    virtual WebXImage * convert(XImage * image, bool hasAlphaChannel) const;

    void handleJPGByte(RawData * rawData, unsigned char oneByte);

};


#endif /* WEBX_JPG_IMAGE_CONVERTER_H */