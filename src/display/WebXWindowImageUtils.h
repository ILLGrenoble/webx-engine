#ifndef WEBX_WINDOW_IMAGE_UTILS_H
#define WEBX_WINDOW_IMAGE_UTILS_H

#include <stdlib.h>
#include <spdlog/spdlog.h>
#include <X11/Xlib.h>
#include <utils/WebXImageUtils.h>
#include <utils/WebXRectangle.h>
#include <image/WebXImage.h>

inline bool checkTransparent(XImage * image) {
    if (image->depth == 24) {
        return false;
    }
    
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    const u_int32_t * data = (u_int32_t *)image->data;

    const size_t width = image->width;
    const size_t height = image->height;
    const size_t length = width * height;

    int transparencyCount = webx_countTransparentPixels(data, length, true);

    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> duration = end - start;

    spdlog::trace("Transparency check {:d} x {:d} ({:d}KB) in {:f}us", width, height, length * 4 / 1024, duration.count());

    return transparencyCount > 0;
}

inline uint32_t calculateImageChecksum(std::shared_ptr<WebXImage> image) {
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    uint32_t checksum = image->getRawChecksum();

    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> duration = end - start;
    spdlog::trace("Checksum for window image {:d} x {:d} ({:d} bytes) in {:f}us", image->getWidth(), image->getHeight(), image->getRawDataSize(), duration.count());
    return checksum;
}

inline uint32_t calculateAlphaChecksum(std::shared_ptr<WebXImage> image) {
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    uint32_t checksum = image->getAlphaChecksum();

    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> duration = end - start;
    spdlog::trace("Checksum for window alpha {:d} x {:d} ({:d} bytes) in {:f}us", image->getWidth(), image->getHeight(), image->getAlphaDataSize(), duration.count());
    return checksum;
}

inline bool makeOpaque(XImage * image, WebXRectangle * mainWindowRectangle, WebXRectangle * subWindowRectangle, WebXRectangle * imageRectangle) {
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    // Do nothing if image depth is already 32bpp or subwindow rectangle same as main rectangle
    if (subWindowRectangle == NULL) {
        return false;
    
    } else if (
        subWindowRectangle->x == 0 && 
        subWindowRectangle->y == 0 && 
        subWindowRectangle->size.width == mainWindowRectangle->size.width && 
        subWindowRectangle->size.height == mainWindowRectangle->size.height) {
        return false;
    
    } else if (image->depth == 32) {
        return false;
    
    } else if (imageRectangle != NULL && subWindowRectangle->contains(*imageRectangle)) {
        return false;
    }

    int startX = subWindowRectangle->x > imageRectangle->x ? subWindowRectangle->x - imageRectangle->x : 0;
    int startY = subWindowRectangle->y > imageRectangle->y ? subWindowRectangle->y - imageRectangle->y : 0;

    int endX = startX + subWindowRectangle->size.width < image->width ? startX + subWindowRectangle->size.width : image->width;
    int endY = startY + subWindowRectangle->size.height < image->height ? startY + subWindowRectangle->size.height : image->height;

    int width = endX - startX;
    int height = endY - startY;

    for (int j = startY; j < endY; j++) {
        u_int32_t * data = (u_int32_t *)image->data + (j * image->width) + startX;
        for (int i = startX; i < endX; i++) {
            *data = *data | 0xFF000000;
            data++;
        }
    }

    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> duration = end - start;
    spdlog::trace("Make Opaque {:d} x {:d} ({:d} pixels) in {:f}us", width, height, width * height, duration.count());

    return true;
}

#endif /* WEBX_WINDOW_IMAGE_UTILS_H */