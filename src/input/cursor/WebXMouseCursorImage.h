#ifndef WEBX_MOUSE_CURSOR_IMAGE_H
#define WEBX_MOUSE_CURSOR_IMAGE_H

#include "utils/WebXDataBuffer.h"
#include <string>

class WebXMouseCursorImage {
public:
    /**
     * Create a new mouse cursor image
     * @param width the width of the cursor
     * @param height the height of the cursor
     * @param rawData the cursor image data
     * @param encodingTimeUs  the time taken to convert into an image
     */
    WebXMouseCursorImage(unsigned int width, unsigned int height, WebXDataBuffer *rawData, double encodingTimeUs);

    virtual ~WebXMouseCursorImage();

    /**
     * Get the image width
     * @return the width
     */
    unsigned int getWidth() const {
        return this->_width;
    }

    /**
     * Get the image height
     * @return the height
     */
    unsigned int getHeight() const {
        return this->_height;
    }

    /**
     * Get the image raw data
     * @return the rawdata
     */
    unsigned char * getRawData() const {
        return this->_rawData ? this->_rawData->getBuffer() : NULL;
    }

    /**
     * Get the raw data size
     * @return  the raw data size
     */
    size_t getRawDataSize() const {
        return this->_rawData ? this->_rawData->getBufferSize() : 0;
    }

    /**
     * Get the time taken to convert the cursor into an image
     * @return the time taken
     */
    double getEncodingTimeUs() const {
        return this->_encodingTimeUs;
    }

    /**
     * Save the image to a file
     * @param fileName the filename to be saved to
     * @return true if successful, otherwise false
     */
    bool save(const std::string & fileName);


private:
    unsigned int _width;
    unsigned int _height;
    double _encodingTimeUs;
    WebXDataBuffer *_rawData;
};


#endif //WEBX_MOUSE_CURSOR_IMAGE_H
