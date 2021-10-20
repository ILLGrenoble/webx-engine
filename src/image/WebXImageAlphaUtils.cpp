#include "WebXImageAlphaUtils.h"
#include <spdlog/spdlog.h>
#include <chrono>

bool WebXImageAlphaUtils::convert(XImage * image, WebXRectangle * mainWindowRectangle, WebXRectangle * subWindowRectangle, WebXRectangle * imageRectangle) {
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

    for (int j = startY; j < endY; j++) {
        u_int8_t * data = (u_int8_t *)image->data + (j * image->bytes_per_line) + (4 * startX) + 3;
        for (int i = startX; i < endX; i++) {
            *data = 0xff;
            data += 4;
        }
    }

    return true;
}

bool WebXImageAlphaUtils::hasAlpha(XImage * image, const WebXRectangle & rectangle) {
    if (image->depth == 24) {
        return false;
    }

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    int startX = rectangle.x;
    int startY = rectangle.y;

    int endX = startX + rectangle.size.width;
    int endY = startY + rectangle.size.height;

    bool hasAlpha = false;

    // Use 32bit values to compare alpha values rather than individual bytes: performance approx x20000 compared to code commented below.
    int pixelsPerLine = image->bytes_per_line / 4;
    for (int j = startY; j < endY && !hasAlpha; j++) {
        u_int32_t * data = (u_int32_t *)image->data + (j * pixelsPerLine) + startX;
        for (int i = startX; i < endX && !hasAlpha; i++) {
            if ((*data & 0xFF000000) != 0xFF000000) {
                hasAlpha = true;
            }
            data++;
        }
    }

    // for (int j = startY; j < endY && !hasAlpha; j++) {
    //     u_int8_t * data = (u_int8_t *)image->data + (j * image->bytes_per_line) + (4 * startX) + 3;
    //     for (int i = startX; i < endX && !hasAlpha; i++) {
    //         if (*data != 0xff) {
    //             hasAlpha = true;
    //         }
    //         data += 4;
    //     }
    // }

    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> duration = end - start;
    spdlog::trace("Checked if image has alpha ({:b}) for image of {:d} x {:d} ({:d} pixels) in {:f}us", hasAlpha, rectangle.size.width, rectangle.size.height, rectangle.size.width * rectangle.size.height, duration.count());

    return hasAlpha;
}
