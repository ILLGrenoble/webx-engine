#ifndef WEBX_WEBP_IMAGE_CONVERTER_H
#define WEBX_WEBP_IMAGE_CONVERTER_H

#include "WebXImageConverter.h"
#include <stdlib.h>

class WebXImage;

/*
 * WebXWebPImageConverter
 * 
 * A concrete implementation of the `WebXImageConverter` interface for converting
 * images into the WebP format.
 */
class WebXWebPImageConverter : public WebXImageConverter {
public:
    /*
     * Constructor.
     */
    WebXWebPImageConverter();

    /*
     * Destructor.
     */
    virtual ~WebXWebPImageConverter();

    /*
     * Converts an XImage object into a WebXImage object in WebP format.
     * 
     * @param image: Pointer to the XImage object to be converted.
     * @param quality: Quality settings for the conversion.
     * @return Pointer to the converted WebXImage object.
     */
    virtual WebXImage * convert(XImage * image, const WebXQuality & quality) const;

    /*
     * Converts raw image data into a WebXImage object in WebP format.
     * 
     * @param data: Pointer to the raw image data.
     * @param width: Width of the image.
     * @param height: Height of the image.
     * @param bytesPerLine: Number of bytes per line in the image data.
     * @param imageDepth: Depth of the image.
     * @param quality: Quality settings for the conversion.
     * @return Pointer to the converted WebXImage object.
     */
    virtual WebXImage * convert(unsigned char * data, int width, int height, int bytesPerLine, int imageDepth, const WebXQuality & quality) const;
};

#endif /* WEBX_WEBP_IMAGE_CONVERTER_H */