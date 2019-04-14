#include "WebXPNGImageConverter.h"
#include "WebXPNGImage.h"
#include <stdlib.h>
#include <cstring>

void WebXPNGImageConverter::RawDataWriter(png_struct * png, png_byte * data, size_t length) {
    // https://stackoverflow.com/questions/1821806/how-to-encode-png-to-buffer-using-libpng
    /* with libpng15 next line causes pointer deference error; use libpng12 */
    WebXPNGImageConverter::RawData * rawData = (WebXPNGImageConverter::RawData *)png_get_io_ptr(png); /* was png_ptr->io_ptr */
    size_t nsize = rawData->size + length;

    /* allocate or grow buffer */
    if(rawData->buffer) {
        rawData->buffer = (unsigned char *)realloc(rawData->buffer, nsize);
    } else {
        rawData->buffer = (unsigned char *)malloc(nsize);
    }

    if (!rawData->buffer) {
        png_error(png, "Write Error");
    }

    /* copy new bytes to end of buffer */
    memcpy(rawData->buffer + rawData->size, data, length);
    rawData->size += length;
}


WebXPNGImageConverter::WebXPNGImageConverter() {
}

WebXPNGImageConverter::~WebXPNGImageConverter() {

}

WebXImage * WebXPNGImageConverter::convert(XImage * image, bool hasAlphaChannel) const {

    png_struct * png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        return NULL;
    }

    png_info * pngInfo = png_create_info_struct(png);
    if (!pngInfo) {
        png_destroy_write_struct(&png, (png_info **)NULL);
        return NULL;
    }

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &pngInfo);
        return NULL;
    }

    // Determine bit depth and if we need to convert alpha
    int image_depth = hasAlphaChannel ? 32 : image->depth;

    RawData rawData;
    png_set_write_fn(png, &rawData, WebXPNGImageConverter::RawDataWriter, NULL);

    // Output is 8bit/channel depth, RGBA format.
    png_set_IHDR(png, pngInfo,
                 image->width, image->height,
                 8, // depth
                 image_depth == 32 ? PNG_COLOR_TYPE_RGBA : PNG_COLOR_TYPE_RGB,
                 PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_DEFAULT,
                 PNG_FILTER_TYPE_DEFAULT);
    png_write_info(png, pngInfo);

    // To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
    // Use png_set_filler().
    if (image_depth == 24) {
        png_set_filler(png, 0, PNG_FILLER_AFTER);
    }
    png_set_bgr(png); // RGB > BGR if needed, we need this

    // write image data
    png_byte **row_pointers = NULL; // png_byte *row_pointers[height]

    u_int8_t * image_data_ptr = (u_int8_t *)image->data;

    row_pointers = (png_byte **)malloc(sizeof(png_byte *) * image->height);
    if (row_pointers) {
        for (int i = 0; i < image->height; i++) {
            // directly point each row into XImage data
            row_pointers[i] = image_data_ptr;
            image_data_ptr += image->bytes_per_line; // move to next row
        }

        // printf("Producing PNG with depth of %dbpp\n", image_depth);

        png_write_image(png, row_pointers);
        free(row_pointers);
    }

    png_write_end(png, pngInfo);

    if (pngInfo) {
        png_free_data(png, pngInfo, PNG_FREE_ALL, -1);
    }

    if (png) {
        png_destroy_write_struct(&png, (png_info **)NULL);
    }

    WebXImage * webXImage = new WebXPNGImage(image->width, image->height, rawData.buffer, rawData.size, image_depth);
    return webXImage;
}
