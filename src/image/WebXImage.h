#ifndef WEBX_IMAGE_H
#define WEBX_IMAGE_H

#include <X11/Xlib.h>
#include <string>
#include <utils/WebXDataBuffer.h>
#include <spdlog/spdlog.h>

class WebXDataBuffer;

/*
 * Enumeration for supported image types.
 */
typedef enum {
    WebXImageTypePNG = 0,
    WebXImageTypeJPG,
    WebXImageTypeJXL,
    WebXImageTypeWebP
} WebXImageType;

/*
 * WebXImage
 * 
 * Represents an image in the WebX system, including its metadata and raw/alpha data.
 */
class WebXImage {
public:
    /*
     * Constructor for images without alpha data.
     * 
     * @param type: The type of the image (e.g., PNG, JPG, WebP).
     * @param width: The width of the image.
     * @param height: The height of the image.
     * @param rawData: Pointer to the raw image data buffer.
     * @param depth: The depth of the image.
     * @param encodingTimeUs: The time taken to encode the image, in microseconds.
     */
    WebXImage(WebXImageType type, unsigned int width, unsigned int height, WebXDataBuffer * rawData, unsigned int depth, double encodingTimeUs);

    /*
     * Constructor for images with alpha data.
     * 
     * @param type: The type of the image (e.g., PNG, JPG, WebP).
     * @param width: The width of the image.
     * @param height: The height of the image.
     * @param rawData: Pointer to the raw image data buffer.
     * @param alphaData: Pointer to the alpha data buffer.
     * @param depth: The depth of the image.
     * @param encodingTimeUs: The time taken to encode the image, in microseconds.
     */
    WebXImage(WebXImageType type, unsigned int width, unsigned int height, WebXDataBuffer * rawData, WebXDataBuffer * alphaData, unsigned int depth, double encodingTimeUs);

    /*
     * Destructor.
     */
    virtual ~WebXImage();

    /*
     * Returns the type of the image.
     */
    WebXImageType getType() const {
        return this->_type;
    }

    /*
     * Returns the width of the image.
     */
    unsigned int getWidth() const {
        return this->_width;
    }

    /*
     * Returns the height of the image.
     */
    unsigned int getHeight() const {
        return this->_height;
    }

    /*
     * Returns the depth of the image.
     */
    unsigned int getDepth() const {
        return this->_depth;
    }

    /*
     * Returns a pointer to the raw image data buffer.
     */
    unsigned char * getRawData() const {
        return this->_rawData ? this->_rawData->getBuffer() : NULL;
    }

    /*
     * Returns the size of the raw image data buffer.
     */
    size_t getRawDataSize() const {
        return this->_rawData ? this->_rawData->getBufferSize() : 0;
    }

    /*
     * Returns a pointer to the alpha data buffer.
     */
    unsigned char * getAlphaData() const {
        return this->_rawData ? this->_alphaData->getBuffer() : NULL;
    }

    /*
     * Returns the size of the alpha data buffer.
     */
    size_t getAlphaDataSize() const {
        return this->_alphaData ? this->_alphaData->getBufferSize() : 0;
    }

    /*
     * Returns the combined size of the raw and alpha data buffers.
     */
    size_t getFullDataSize() const {
        return this->getRawDataSize() + this->getAlphaDataSize();
    }

    /*
     * Returns the time taken to encode the image, in microseconds.
     */
    double getEncodingTimeUs() const {
        return this->_encodingTimeUs;
    }

    /*
     * Saves the image to a file.
     * 
     * @param fileName: The name of the file to save the image to.
     * @return True if the image was saved successfully, false otherwise.
     */
    bool save(const std::string & fileName);

    /*
     * Returns the file extension for the image type.
     */
    std::string getFileExtension() const {
        if (this->_type == WebXImageTypePNG) {
            return "png";
        } else if (this->_type == WebXImageTypeJPG) {
            return "jpg";
        } else if (this->_type == WebXImageTypeJXL) {
            return "jxl";
        } else if (this->_type == WebXImageTypeWebP) {
            return "webp";    
        } else {
            return "img";
        }
    }

    /*
     * Calculates and returns the checksum of the raw image data.
     */
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

    /*
     * Calculates and returns the checksum of the alpha data.
     */
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

    /*
     * Removes the alpha data from the image.
     * 
     * @return True if the alpha data was removed successfully, false otherwise.
     */
    bool removeAlpha() {
        if (this->_alphaData) {
            delete this->_alphaData;
            this->_alphaData = NULL;
            return true;
        }
        return false;
    }

private:
    /*
     * Saves a data buffer to a file.
     * 
     * @param dataBuffer: Pointer to the data buffer to save.
     * @param fileName: The name of the file to save the data to.
     * @return True if the data was saved successfully, false otherwise.
     */
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