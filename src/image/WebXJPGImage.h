#ifndef WEBX_JPG_IMAGE_H
#define WEBX_PNG_IMAGE_H

#include "WebXImage.h"

class WebXJPGImage : public WebXImage {
public:
    WebXJPGImage(unsigned int width, unsigned int height, unsigned char * rawData, size_t length, unsigned int depth, double encodingTimeUs);
    virtual ~WebXJPGImage();

    virtual bool save(const std::string & fileName);

    virtual unsigned char * getRawData() const {
        return this->_rawData;
    }

    virtual size_t getRawDataSize() const {
        return this->_length;
    }


private:
     unsigned char * _rawData;
     size_t _length;
};


#endif /* WEBX_PNG_IMAGE_H */