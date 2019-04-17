#ifndef WEBX_IMAGE_H
#define WEBX_IMAGE_H

#include <X11/Xlib.h>
#include <string>
#include <utils/WebXDataBuffer.h>

class WebXDataBuffer;

typedef enum {
    WebXImageTypePNG = 0,
    WebXImageTypeJPG,
    WebXImageTypeWebP
} WebXImageType;

class WebXImage {
public:
    WebXImage(WebXImageType type, unsigned int width, unsigned int height, WebXDataBuffer * rawData, unsigned int depth, double encodingTimeUs);
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

    unsigned char * getRawData() const {
        return this->_rawData ? this->_rawData->getBuffer() : NULL;
    }

    size_t getRawDataSize() const {
        return this->_rawData ? this->_rawData->getBufferSize() : 0;
    }

    double getEncodingTimeUs() const {
        return this->_encodingTimeUs;
    }

    bool save(const std::string & fileName);

    std::string getFileExtension() const {
        if (this->_type == WebXImageTypePNG) {
            return "png";
        } else if (this->_type == WebXImageTypeJPG) {
            return "jpg";
        } else if(this->_type == WebXImageTypeWebP) {
            return "webp";    
        }else {
            return "img";
        }
    }

private:
    WebXImageType _type;
    unsigned int _width;
    unsigned int _height;

    WebXDataBuffer * _rawData;

    unsigned int _depth;

    double _encodingTimeUs;

};


#endif /* WEBX_IMAGE_CONVERTER_H */