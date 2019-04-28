#include "WebXMouseCursorImageConverter.h"
#include <png.h>
#include <stdlib.h>
#include <chrono>

void WebXMouseCursorImageConverter::RawDataWriter(png_struct *png, png_byte *data, size_t length) {
    // https://stackoverflow.com/questions/1821806/how-to-encode-png-to-buffer-using-libpng
    /* with libpng15 next line causes pointer deference error; use libpng12 */
    WebXDataBuffer *rawData = (WebXDataBuffer *) png_get_io_ptr(png);
    rawData->appendData(data, length);
}

WebXMouseCursorImageConverter::WebXMouseCursorImageConverter() {
}

WebXMouseCursorImageConverter::~WebXMouseCursorImageConverter() {
}

WebXMouseCursorImage * WebXMouseCursorImageConverter::convert(XFixesCursorImage * image) const {
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    png_struct * png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_info * pngInfo = png_create_info_struct(png);

    WebXDataBuffer *rawData = new WebXDataBuffer(1024);
    png_set_write_fn(png, rawData, WebXMouseCursorImageConverter::RawDataWriter, NULL);

    png_set_IHDR(png,
                 pngInfo,
                 image->width,
                 image->height, 8,
                 PNG_COLOR_TYPE_RGB_ALPHA,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, pngInfo);

    unsigned long *src = image->pixels;
    png_byte **rows = (png_byte **)malloc(sizeof(png_byte *) * image->height);
    for (unsigned long y = 0; y < image->height; y++) {
        rows[y] = static_cast<png_byte *>(malloc(static_cast<size_t>(image->width * 4)));
        for (unsigned long x = 0; x < image->width; x++) {
            uint32_t p = *src++;
            uint8_t r = p >> 0;
            uint8_t g = p >> 8;
            uint8_t b = p >> 16;
            uint8_t a = p >> 24;

            if (a > 0x00 && a < 0xff) {
                r = (r * 0xff + a / 2) / a;
                g = (g * 0xff + a / 2) / a;
                b = (b * 0xff + a / 2) / a;
            }

            rows[y][4 * x + 0] = b;
            rows[y][4 * x + 1] = g;
            rows[y][4 * x + 2] = r;
            rows[y][4 * x + 3] = a;
        }
    }

    png_write_image(png, rows);
    free(rows);
    png_write_end(png, NULL);
    png_free_data(png, pngInfo, PNG_FREE_ALL, -1);
    png_destroy_write_struct(&png, (png_info **) NULL);

    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> duration = end - start;

    return new WebXMouseCursorImage(image->width, image->height, rawData, duration.count());
}
