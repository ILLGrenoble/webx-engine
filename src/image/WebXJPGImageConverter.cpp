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
    return convert((unsigned char *)image->data, image->width, image->height, image->width * 4, image->depth);
}

WebXImage * WebXJPGImageConverter::convert(unsigned char * data, int width, int height, int bytesPerLine, int imageDepth) const {

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    WebXImage * webXImage = nullptr;

    WebXDataBuffer * rawData = this->_convert(data, width, height, bytesPerLine, imageDepth);
    WebXDataBuffer * alphaData = nullptr;

    if (imageDepth == 32) {
        unsigned int imageSize = width * height;

        // Generate alphaMap (reuse original data)
        unsigned char * srcPtr = data + 3; // alpha channel
        unsigned char * destPtr = data;
        for (int i = 0; i < imageSize; i++) {
            *(destPtr) = 0;
            *(destPtr + 1) = *srcPtr;
            *(destPtr + 2) = 0;
            srcPtr += 4;
            destPtr += 4;
        }

        alphaData = this->_convert(data, width, height, bytesPerLine, imageDepth);
    }

    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> duration = end - start;

    webXImage = new WebXImage(WebXImageTypeJPG, width, height, rawData, alphaData, imageDepth, duration.count());

    return webXImage;
}

WebXDataBuffer * WebXJPGImageConverter::_convert(unsigned char * data, int width, int height, int bytesPerLine, int imageDepth) const {

    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW row_pointer[1];
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    
    unsigned char * jpegData = 0;
    unsigned long jpegDataSize = 0;

    jpeg_mem_dest(&cinfo, &jpegData, &jpegDataSize);

    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 4;
    cinfo.in_color_space = JCS_EXT_BGRA;
    
    jpeg_set_defaults(&cinfo);

	cinfo.dct_method = JDCT_IFAST;

    jpeg_set_quality(&cinfo, 90, TRUE);

    jpeg_start_compress(&cinfo, TRUE);

    unsigned char * imageData = (unsigned char *)data;
    unsigned char row_buf[4096];

    unsigned long offset;
    row_pointer[0] = row_buf;
    unsigned int i;
    unsigned int j;
    while (cinfo.next_scanline < cinfo.image_height) {
        offset = cinfo.next_scanline * bytesPerLine;
        row_pointer[0] = &imageData[offset];
        (void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);

    WebXDataBuffer * rawData = new WebXDataBuffer(jpegData, jpegDataSize);
    return rawData;
}
