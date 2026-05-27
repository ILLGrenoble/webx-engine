#ifndef WEBX_PIXEL_BUFFER_H
#define WEBX_PIXEL_BUFFER_H

class WebXPixelBuffer {
public:
    WebXPixelBuffer(char * pixels, int width, int height, int bytesPerLine, int colorDepth) :
        pixels(pixels),
        width(width),
        height(height),
        bytesPerLine(bytesPerLine),
        colorDepth(colorDepth) {

        }
    
    WebXPixelBuffer(const WebXPixelBuffer & buffer) :
        pixels(buffer.pixels),
        width(buffer.width),
        height(buffer.height),
        bytesPerLine(buffer.bytesPerLine),
        colorDepth(buffer.colorDepth) {

    }
public:
    const char * pixels;
    const int width;
    const int height;
    const int bytesPerLine;
    const int colorDepth; // 24 or 32
};

#endif /* WEBX_PIXEL_BUFFER_H */