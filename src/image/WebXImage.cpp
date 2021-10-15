#include "WebXImage.h"

WebXImage::WebXImage(WebXImageType type, unsigned int width, unsigned int height, WebXDataBuffer * rawData, unsigned int depth, double encodingTimeUs) :
    _type(type),
    _width(width),
    _height(height),
    _rawData(rawData),
    _alphaData(0),
    _checksum(0),
    _depth(depth),
    _encodingTimeUs(encodingTimeUs) {
}

WebXImage::WebXImage(WebXImageType type, unsigned int width, unsigned int height, WebXDataBuffer * rawData, WebXDataBuffer * alphaData, unsigned int depth, double encodingTimeUs) :
    _type(type),
    _width(width),
    _height(height),
    _rawData(rawData),
    _alphaData(alphaData),
    _checksum(0),
    _depth(depth),
    _encodingTimeUs(encodingTimeUs) {
}

WebXImage::~WebXImage() {
    if (this->_rawData) {
        delete this->_rawData;
        this->_rawData = NULL;
    }
    if (this->_alphaData) {
        delete this->_alphaData;
        this->_alphaData = NULL;
    }
}

bool WebXImage::save(const std::string & fileName) {
    if (this->getRawData() != NULL) {
        FILE * fp = fopen(fileName.c_str(), "wb");

        if (!fp) {
            return false;
        }

        fwrite(this->getRawData(), this->getRawDataSize(), 1, fp);

        fclose(fp);

        return true;

    } else {
        return false;
    }
}
