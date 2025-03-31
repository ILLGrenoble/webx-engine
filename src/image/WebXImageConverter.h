#ifndef WEBX_IMAGE_CONVERTER_H
#define WEBX_IMAGE_CONVERTER_H

#include <X11/Xlib.h>
#include <spdlog/spdlog.h>
#include <models/WebXQuality.h>

class WebXImage;

/*
 * WebXImageConverter
 * 
 * Abstract base class for image conversion. Derived classes must implement the
 * `convert` methods to handle specific image formats or conversion logic.
 */
class WebXImageConverter {
public:
    /*
     * Default constructor.
     */
    WebXImageConverter() {}

    /*
     * Virtual destructor.
     */
    virtual ~WebXImageConverter() {}

    /*
     * Converts an XImage object into a WebXImage object.
     * 
     * @param image: Pointer to the XImage object to be converted.
     * @param quality: Quality settings for the conversion.
     * @return Pointer to the converted WebXImage object.
     */
    virtual WebXImage * convert(XImage * image, const WebXQuality & quality) const = 0;

    /*
     * Converts raw image data into a WebXImage object.
     * 
     * @param data: Pointer to the raw image data.
     * @param width: Width of the image.
     * @param height: Height of the image.
     * @param bytesPerLine: Number of bytes per line in the image data.
     * @param imageDepth: Depth of the image.
     * @param quality: Quality settings for the conversion.
     * @return Pointer to the converted WebXImage object.
     */
    virtual WebXImage * convert(unsigned char * data, int width, int height, int bytesPerLine, int imageDepth, const WebXQuality & quality) const = 0;
};

#endif /* WEBX_IMAGE_CONVERTER_H */