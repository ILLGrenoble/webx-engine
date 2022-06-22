#include "WebXWebPImageConverter.h"
#include "WebXImage.h"
#include <cstring>
#include <chrono>
#include <webp/encode.h>

WebXWebPImageConverter::WebXWebPImageConverter() {
}

WebXWebPImageConverter::~WebXWebPImageConverter() {
}

WebXImage * WebXWebPImageConverter::convert(XImage * image, float quality) const {
    return convert((unsigned char *)image->data, image->width, image->height, image->width * 4, image->depth, quality);
}

WebXImage * WebXWebPImageConverter::convert(unsigned char * data, int width, int height, int bytesPerLine, int imageDepth, float quality) const {

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    uint8_t *output = 0;
    size_t size = WebPEncodeBGRA((uint8_t *)data, width, height, bytesPerLine, (quality == 0 ? this->_quality : quality) * 100, &output);
    // size_t size = WebPEncodeLosslessBGRA((uint8_t *)image->data, image->width, image->height, bytesPerLine, &output);

    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> duration = end - start;

    WebXDataBuffer *rawData = new WebXDataBuffer(output, size);

    WebPFree(output);

    WebXImage *webXImage = new WebXImage(WebXImageTypeWebP, width, height, rawData, imageDepth, duration.count());
    return webXImage;
}
