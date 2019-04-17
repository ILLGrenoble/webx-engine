#include "WebXWebPImageConverter.h"
#include "WebXImage.h"
#include <cstring>
#include <chrono>
#include <webp/encode.h>

WebXWebPImageConverter::WebXWebPImageConverter()
{
}

WebXWebPImageConverter::~WebXWebPImageConverter()
{
}

WebXImage *WebXWebPImageConverter::convert(XImage *image, bool hasAlphaChannel) const
{

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    uint8_t *output = 0;
    int stride = 4 * image->width;
    int quality = 80;
    size_t size = WebPEncodeBGRA((uint8_t *)image->data, image->width, image->height, stride, quality, &output);
    // size_t size = WebPEncodeLosslessBGRA((uint8_t *)image->data, image->width, image->height, stride, &output);

    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> duration = end - start;

    WebXDataBuffer *rawData = new WebXDataBuffer(output, size);

    WebPFree(output);

    WebXImage *webXImage = new WebXImage(WebXImageTypeWebP, image->width, image->height, rawData, image->depth, duration.count());
    return webXImage;
}
