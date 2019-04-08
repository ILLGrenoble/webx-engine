#ifndef WEBX_PNG_IMAGE_H
#define WEBX_PNG_IMAGE_H

#include "WebXImage.h"

class WebXPNGImage : public WebXImage {
public:
    WebXPNGImage(unsigned int width, unsigned int height, unsigned char * rawData, size_t length, unsigned int depth);
    virtual ~WebXPNGImage();

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