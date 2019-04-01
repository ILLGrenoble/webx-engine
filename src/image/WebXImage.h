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
    WebXImage(WebXImageType type, unsigned int width, unsigned int height);
    virtual ~WebXImage();

    WebXImageType getType() const {
        return this->_type;
    }

    unsigned int getWidth() const {
        return this->_width;
    }

    unsigned int getHeiht() const {
        return this->_height;
    }

    virtual bool save(const std::string & fileName) = 0;

private:
    WebXImageType _type;
    unsigned int _width;
    unsigned int _height;

};


#endif /* WEBX_IMAGE_CONVERTER_H */