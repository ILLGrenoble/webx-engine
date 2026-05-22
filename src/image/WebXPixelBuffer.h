#ifndef WEBX_PIXEL_BUFFER_H
#define WEBX_PIXEL_BUFFER_H

struct WebXPixelBuffer {
    char * pixels;
    int width;
    int height;
    int bytesPerLine;
    int colorDepth; // 24 or 32
};

#endif /* WEBX_PIXEL_BUFFER_H */