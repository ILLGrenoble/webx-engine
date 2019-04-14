#include "WebXImageAlphaConverter.h"

bool WebXImageAlphaConverter::convert(XImage * image, WebXRectangle * mainWindowRectangle, WebXRectangle * subWindowRectangle, WebXRectangle * imageRectangle) {
    // Do nothing if image depth is already 32bpp or subwindow rectangle same as main rectangle
    if (subWindowRectangle == NULL ||
        (subWindowRectangle->x == 0 && 
        subWindowRectangle->y == 0 && 
        subWindowRectangle->size.width == mainWindowRectangle->size.width && 
        subWindowRectangle->size.height == mainWindowRectangle->size.height) ||
        image->depth == 32) {
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
