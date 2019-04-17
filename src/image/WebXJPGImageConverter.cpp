#include "WebXJPGImageConverter.h"
#include "WebXImage.h"
#include <jpeglib.h>
#include <cstring>
#include <chrono>

WebXJPGImageConverter::WebXJPGImageConverter() {
}

WebXJPGImageConverter::~WebXJPGImageConverter() {

}

WebXImage * WebXJPGImageConverter::convert(XImage * image, bool hasAlphaChannel) const {

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW row_pointer[1];
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    
    unsigned char * jpegData = 0;
    unsigned long jpegDataSize = 0;

    jpeg_mem_dest(&cinfo, &jpegData, &jpegDataSize);

    cinfo.image_width = image->width;
    cinfo.image_height = image->height;
    cinfo.input_components = 4;
    cinfo.in_color_space = JCS_EXT_BGRA;
    
    jpeg_set_defaults(&cinfo);

	cinfo.dct_method = JDCT_IFAST;

    jpeg_set_quality(&cinfo, 90, TRUE);

    jpeg_start_compress(&cinfo, TRUE);

    int bytes_per_line = image->width * 4;

    unsigned char * imageData = (unsigned char *)image->data;
    unsigned char row_buf[4096];

    unsigned long offset;
    row_pointer[0] = row_buf;
    unsigned int i;
    unsigned int j;
    while (cinfo.next_scanline < cinfo.image_height) {
        offset = cinfo.next_scanline * bytes_per_line;
        row_pointer[0] = &imageData[offset];
        (void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

    WebXDataBuffer * rawData = new WebXDataBuffer(jpegData, jpegDataSize);

    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> duration = end - start;

    WebXImage * webXImage = new WebXImage(WebXImageTypeJPG, image->width, image->height, rawData, image->depth, duration.count());
    return webXImage;
}
