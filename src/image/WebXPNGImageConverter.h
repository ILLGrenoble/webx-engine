#ifndef WEBX_PNG_IMAGE_CONVERTER_H
#define WEBX_PNG_IMAGE_CONVERTER_H

#include "WebXImageConverter.h"
#include <png.h>
#include <stdlib.h>

/**
 * @class WebXPNGImageConverter
 * @brief Converts raw image data into PNG format.
 * 
 * This class provides methods to convert raw image data into PNG format
 * using the libpng library. It supports both full window and sub-region conversions.
 */
class WebXImage;

class WebXPNGImageConverter : public WebXImageConverter {
public:
    /**
     * @brief Constructs a WebXPNGImageConverter object.
     */
    WebXPNGImageConverter();

    /**
     * @brief Destructor for WebXPNGImageConverter.
     */
    virtual ~WebXPNGImageConverter();

    /**
     * @brief Converts an XImage object into a WebXImage in PNG format.
     * 
     * @param image The XImage object to convert.
     * @param quality The quality settings for the conversion.
     * @return A pointer to the converted WebXImage object.
     */
    virtual WebXImage * convert(XImage * image, const WebXQuality & quality) const;

    /**
     * @brief Converts raw image data into a WebXImage in PNG format.
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
     * @brief A helper function to write raw PNG data.
     * 
     * @param png The PNG write structure.
     * @param data The data to write.
     * @param length The length of the data.
     */
    static void RawDataWriter(png_struct * png, png_byte * data, png_size_t length);

private:
    class RawData {
    public:
        RawData() :
            buffer(0),
            capacity(0),
            size(0) {}
        RawData(size_t initialCapacity) :
            buffer(0),
            capacity(initialCapacity),
            size(0) {
            buffer = (unsigned char *)malloc(initialCapacity);
        }
        virtual ~RawData() {}

        unsigned char * buffer;
        size_t capacity;
        size_t size;
    };

};


#endif /* WEBX_PNG_IMAGE_CONVERTER_H */