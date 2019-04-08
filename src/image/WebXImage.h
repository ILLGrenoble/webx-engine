#ifndef WEBX_IMAGE_H
#define WEBX_IMAGE_H

#include <X11/Xlib.h>
#include <string>

typedef enum {
  WebXImageTypePNG = 0,
    WebXImageTypeJPG
} WebXImageType;

class WebXImage {
public:
    WebXImage(WebXImageType type, unsigned int width, unsigned int height, unsigned int depth);
    virtual ~WebXImage();

    WebXImageType getType() const {
        return this->_type;
    }

    unsigned int getWidth() const {
        return this->_width;
    }

    unsigned int getHeight() const {
        return this->_height;
    }

    unsigned int getDepth() const {
        return this->_depth;
    }

    virtual bool save(const std::string & fileName) = 0;

    virtual unsigned char * getRawData() const = 0;
    virtual size_t getRawDataSize() const = 0;

private:
    WebXImageType _type;
    unsigned int _width;
    unsigned int _height;
    unsigned int _depth;

};


#endif /* WEBX_IMAGE_CONVERTER_H */