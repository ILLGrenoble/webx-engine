#include "WebXJPGImageConverter.h"
#include "WebXImage.h"
#include <utils/WebXImageUtils.h>
#include <jpeglib.h>
#include <cstring>
#include <chrono>
#include <spdlog/spdlog.h>

WebXJPGImageConverter::WebXJPGImageConverter() {
}

WebXJPGImageConverter::~WebXJPGImageConverter() {

}

WebXImage * WebXJPGImageConverter::convert(XImage * image) const {
    return convert((unsigned char *)image->data, image->width, image->height, image->width * 4, image->depth);
}

WebXImage * WebXJPGImageConverter::convert(unsigned char * data, int width, int height, int bytesPerLine, int imageDepth) const {

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    WebXImage * webXImage = nullptr;

    WebXDataBuffer * rawData = this->_convert(data, width, height, bytesPerLine);
    WebXDataBuffer * alphaData = nullptr;

    if (imageDepth == 32) {
        unsigned int imageSize = width * height;

        std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

        // // Generate alphaMap (reuse original data): remove all other components (keep only alpha)
        for (int i = 0; i < 10; i++) {
            webx_convertToAlpha((u_int32_t *)data, imageSize);
        }

        std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::micro> duration = end - start;
        spdlog::trace("Converted raw image data for alpha map jpeg creation {:d} x {:d} ({:d} pixels) in {:f}us", width, height,width * height, duration.count());

        // Generate alphaMap: offset data pointer so that alpha is aligned with expected green component (green used by three.js in alphaMap)
        alphaData = this->_convert(data + 2, width, height, bytesPerLine);
    }

    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> duration = end - start;

    webXImage = new WebXImage(WebXImageTypeJPG, width, height, rawData, alphaData, imageDepth, duration.count());

    return webXImage;
}

WebXDataBuffer * WebXJPGImageConverter::_convert(unsigned char * data, int width, int height, int bytesPerLine) const {

    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
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

    JSAMPROW * row_pointer = (JSAMPROW *)malloc(sizeof(JSAMPROW) * height);
    for (int i = 0; i < height; i++) {
        row_pointer[i] = (JSAMPROW)&data[i * bytesPerLine];
    }

    jpeg_start_compress(&cinfo, TRUE);
    while (cinfo.next_scanline < cinfo.image_height) {
        jpeg_write_scanlines(&cinfo, &row_pointer[cinfo.next_scanline], cinfo.image_height - cinfo.next_scanline);
    }
    jpeg_finish_compress(&cinfo);

    jpeg_destroy_compress(&cinfo);

    free(row_pointer);

    WebXDataBuffer * rawData = new WebXDataBuffer(jpegData, jpegDataSize);
    return rawData;
}
