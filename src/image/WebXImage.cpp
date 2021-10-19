#include "WebXImage.h"

WebXImage::WebXImage(WebXImageType type, unsigned int width, unsigned int height, WebXDataBuffer * rawData, unsigned int depth, double encodingTimeUs) :
    _type(type),
    _width(width),
    _height(height),
    _rawData(rawData),
    _alphaData(0),
    _rawChecksum(0),
    _alphaChecksum(0),
    _depth(depth),
    _encodingTimeUs(encodingTimeUs) {
}

WebXImage::WebXImage(WebXImageType type, unsigned int width, unsigned int height, WebXDataBuffer * rawData, WebXDataBuffer * alphaData, unsigned int depth, double encodingTimeUs) :
    _type(type),
    _width(width),
    _height(height),
    _rawData(rawData),
    _alphaData(alphaData),
    _rawChecksum(0),
    _alphaChecksum(0),
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

void WebXImage::removeAlpha() {
    if (this->_alphaData) {
        delete this->_alphaData;
        this->_alphaData = NULL;
    }
}

bool WebXImage::save(const std::string & fileName) {
    bool allOk = false;
    if (_rawData != NULL) {
         allOk = this->saveDataBuffer(this->_rawData, fileName);

        if (_alphaData != NULL) {
            allOk &= this->saveDataBuffer(this->_alphaData, fileName + "+alpha");
        }
    }

    return allOk;
}

bool WebXImage::saveDataBuffer(WebXDataBuffer * dataBuffer, const std::string & fileName) {
    if (dataBuffer->getBuffer() != NULL) {
        std::string fullFileName = fileName + "." + this->getFileExtension();

        FILE * fp = fopen(fullFileName.c_str(), "wb");

        if (!fp) {
            return false;
        }

        fwrite(dataBuffer->getBuffer(), dataBuffer->getBufferSize(), 1, fp);

        fclose(fp);

        return true;

    } else {
        return false;
    }
}
