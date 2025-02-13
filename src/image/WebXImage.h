#ifndef WEBX_IMAGE_H
#define WEBX_IMAGE_H

#include <X11/Xlib.h>
#include <string>
#include <utils/WebXDataBuffer.h>
#include <spdlog/spdlog.h>

class WebXDataBuffer;

typedef enum {
    WebXImageTypePNG = 0,
    WebXImageTypeJPG,
    WebXImageTypeWebP
} WebXImageType;

class WebXImage {
public:
    WebXImage(WebXImageType type, unsigned int width, unsigned int height, WebXDataBuffer * rawData, unsigned int depth, double encodingTimeUs);
    WebXImage(WebXImageType type, unsigned int width, unsigned int height, WebXDataBuffer * rawData, WebXDataBuffer * alphaData, unsigned int depth, double encodingTimeUs);
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

    unsigned char * getAlphaData() const {
        return this->_rawData ? this->_alphaData->getBuffer() : NULL;
    }

    size_t getAlphaDataSize() const {
        return this->_alphaData ? this->_alphaData->getBufferSize() : 0;
    }

    size_t getFullDataSize() const {
        return this->getRawDataSize() + this->getAlphaDataSize();
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
        } else if (this->_type == WebXImageTypeWebP) {
            return "webp";    
        }else {
            return "img";
        }
    }

    uint32_t calculateImageChecksum() {
        if (this->_rawChecksum == 0 && this->_rawData != nullptr) {
            std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

            this->_rawChecksum = this->_rawData->calculateChecksum();
        
            std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::micro> duration = end - start;
            spdlog::trace("Checksum for window image {:d} x {:d} ({:d} bytes) in {:f}us", this->_width, this->_height, this->getRawDataSize(), duration.count());
        }
        return this->_rawChecksum;
    }

    uint32_t calculateAlphaChecksum() {
        if (this->_alphaChecksum == 0 && this->_alphaData != nullptr) {
            std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

            this->_alphaChecksum = this->_alphaData->calculateChecksum();
        
            std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::micro> duration = end - start;
            spdlog::trace("Checksum for window alpha {:d} x {:d} ({:d} bytes) in {:f}us", this->_width, this->_height, this->getAlphaDataSize(), duration.count());
        }
        return this->_alphaChecksum;
    }

    bool removeAlpha() {
        if (this->_alphaData) {
            delete this->_alphaData;
            this->_alphaData = NULL;
        return true;
        }
        return false;
    }

private:
    bool saveDataBuffer(WebXDataBuffer * dataBuffer, const std::string & fileName);

private:
    WebXImageType _type;
    unsigned int _width;
    unsigned int _height;

    WebXDataBuffer * _rawData;
    WebXDataBuffer * _alphaData;
    uint32_t _rawChecksum;
    uint32_t _alphaChecksum;

    unsigned int _depth;

    double _encodingTimeUs;

};


#endif /* WEBX_IMAGE_CONVERTER_H */