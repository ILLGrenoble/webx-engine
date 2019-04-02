#ifndef WEBX_PNG_IMAGE_H
#define WEBX_PNG_IMAGE_H

#include "WebXImage.h"

class WebXPNGImage : public WebXImage {
public:
    WebXPNGImage(unsigned int width, unsigned int height, char * rawData, size_t length, unsigned int depth);
    virtual ~WebXPNGImage();

    virtual bool save(const std::string & fileName);

private:
     char * _rawData;
     size_t _length;
};


#endif /* WEBX_PNG_IMAGE_H */