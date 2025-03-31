#ifndef WEBX_JPG_IMAGE_CONVERTER_H
#define WEBX_JPG_IMAGE_CONVERTER_H

#include "WebXImageConverter.h"
#include <stdlib.h>

class WebXImage;
class WebXDataBuffer;

/**
 * @class WebXJPGImageConverter
 * @brief Converts raw image data into JPEG format.
 * 
 * This class provides methods to convert raw image data into JPEG format
 * using the libjpeg-turbo library. It supports both color and alpha channel conversions.
 */
class WebXJPGImageConverter : public WebXImageConverter {
public:
    /**
     * @brief Constructs a WebXJPGImageConverter object.
     */
    WebXJPGImageConverter();

    /**
     * @brief Destructor for WebXJPGImageConverter.
     */
    virtual ~WebXJPGImageConverter();

    /**
     * @brief Converts an XImage object into a WebXImage in JPEG format.
     * 
     * @param image The XImage object to convert.
     * @param quality The quality settings for the conversion.
     * @return A pointer to the converted WebXImage object.
     */
    virtual WebXImage * convert(XImage * image, const WebXQuality & quality) const;

    /**
     * @brief Converts raw image data into a WebXImage in JPEG format.
     * 
     * @param data The raw image data.
     * @param width The width of the image.
     * @param height The height of the image.
     * @param bytesPerLine The number of bytes per line in the image data.
     * @param imageDepth The depth of the image (e.g., 24 or 32 bits).
     * @param quality The quality settings for the conversion.
     * @return A pointer to the converted WebXImage object.
     */
    virtual WebXImage * convert(unsigned char * data, int width, int height, int bytesPerLine, int imageDepth, const WebXQuality & quality) const;

private:
    /**
     * @brief Converts raw image data into a JPEG buffer.
     * 
     * @param data The raw image data.
     * @param width The width of the image.
     * @param height The height of the image.
     * @param bytesPerLine The number of bytes per line in the image data.
     * @param quality The quality level for the conversion.
     * @return A pointer to the WebXDataBuffer containing the JPEG data.
     */
    WebXDataBuffer * _convert(unsigned char * data, int width, int height, int bytesPerLine, float quality) const;

    /**
     * @brief Converts raw grayscale image data into a JPEG buffer.
     * 
     * @param data The raw grayscale image data.
     * @param width The width of the image.
     * @param height The height of the image.
     * @param quality The quality level for the conversion.
     * @return A pointer to the WebXDataBuffer containing the JPEG data.
     */
    WebXDataBuffer * _convertMono(unsigned char * data, int width, int height, float quality) const;
};

#endif /* WEBX_JPG_IMAGE_CONVERTER_H */