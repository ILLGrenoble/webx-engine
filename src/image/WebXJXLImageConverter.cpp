#include "WebXJXLImageConverter.h"
#include "WebXImage.h"
#include <utils/WebXImageUtils.h>
#include <jxl/encode.h>
#include <cstring>
#include <chrono>
#include <spdlog/spdlog.h>

WebXJXLImageConverter::WebXJXLImageConverter() {
}

WebXJXLImageConverter::~WebXJXLImageConverter() {

}

WebXImage * WebXJXLImageConverter::convert(XImage * image, const WebXQuality & quality) const {
    return convert((unsigned char *)image->data, image->width, image->height, image->bytes_per_line, image->depth, quality);
}

WebXImage * WebXJXLImageConverter::convert(unsigned char * data, int width, int height, int bytesPerLine, int imageDepth, const WebXQuality & quality) const {

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    WebXImage * webXImage = nullptr;

    WebXDataBuffer * rawData = this->_convert(data, width, height, bytesPerLine, quality.rgbQuality);

    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> duration = end - start;

    webXImage = new WebXImage(WebXImageTypeJXL, width, height, rawData, nullptr, imageDepth, duration.count());

    return webXImage;
}

WebXImage * WebXJXLImageConverter::convertMono(XImage * image, const WebXQuality & quality) const {

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    WebXImage * webXImage = nullptr;

    WebXDataBuffer * rawData = this->_convertMono((unsigned char *)image->data, image->width, image->height, image->bytes_per_line, quality.rgbQuality);

    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> duration = end - start;

    webXImage = new WebXImage(WebXImageTypeJXL, image->width, image->height, rawData, nullptr, 8, duration.count());

    return webXImage;
}

WebXDataBuffer * WebXJXLImageConverter::_convert(unsigned char * data, int width, int height, int bytesPerLine, float quality) const {
    // Clamp quality (JPEG XL quality: 100 is lossless, 1–99 is lossy)
    float jpegxl_quality = quality < 0 ? 0 : (quality > 0.97 ? 0.97 : quality);
    float distance = 1.0f + (1.0f - jpegxl_quality) * 7.0f;
    // JPEG XL "distance" mode: lower = higher quality. ~1.0 is high quality.

    JxlEncoder* encoder = JxlEncoderCreate(NULL);
    if (!encoder) return nullptr;

    JxlEncoderFrameSettings* frame = JxlEncoderFrameSettingsCreate(encoder, NULL);

    // Use distance-based lossy tuning
    JxlEncoderFrameSettingsSetOption(frame, JXL_ENC_FRAME_SETTING_EFFORT, 5);
    JxlEncoderFrameSettingsSetOption(frame, JXL_ENC_FRAME_SETTING_DECODING_SPEED, 3);
    JxlEncoderSetFrameDistance(frame, distance);

    // Pixel format: check if the raw data is 24-bit or 32-bit
    bool is32 = (bytesPerLine / width) == 4;
    spdlog::info("is32 = {}", is32);

    JxlPixelFormat pixel_format;
    memset(&pixel_format, 0, sizeof(pixel_format));
    pixel_format.num_channels = is32 ? 4 : 3;
    pixel_format.data_type = JXL_TYPE_UINT8;
    pixel_format.endianness = JXL_NATIVE_ENDIAN;
    pixel_format.align = bytesPerLine; // row stride

    // Set basic image info
    JxlBasicInfo info;
    JxlEncoderInitBasicInfo(&info);   // important: sets defaults

    info.xsize = width;
    info.ysize = height;
    info.bits_per_sample = 8;
    info.exponent_bits_per_sample = 0;
    info.num_color_channels = 3;
    info.num_extra_channels = is32 ? 1 : 0;     // alpha channel
    info.alpha_bits = is32 ? 8 : 0;
    info.alpha_exponent_bits = 0;

    if (JXL_ENC_SUCCESS != JxlEncoderSetBasicInfo(encoder, &info)) {
        JxlEncoderDestroy(encoder);
        return nullptr;
    }

    // Set color space (sRGB)
    JxlColorEncoding color_encoding;
    JxlColorEncodingSetToSRGB(&color_encoding, false);
    JxlEncoderSetColorEncoding(encoder, &color_encoding);

    // Add the image frame
    if (JXL_ENC_SUCCESS !=
        JxlEncoderAddImageFrame(frame, &pixel_format, data, (size_t)bytesPerLine * height)) {
        JxlEncoderDestroy(encoder);
        return nullptr;
    }

    JxlEncoderCloseInput(encoder);

    // Output buffer
    size_t buf_size = 64 * 1024;
    uint8_t* output = (uint8_t*)malloc(buf_size);
    size_t written = 0;

    uint8_t* next_out = output;
    size_t avail_out = buf_size;

    JxlEncoderStatus status;

    while ((status = JxlEncoderProcessOutput(encoder, &next_out, &avail_out)) == JXL_ENC_NEED_MORE_OUTPUT) {
        size_t offset = next_out - output;
        buf_size *= 2;
        output = (uint8_t*)realloc(output, buf_size);
        next_out = output + offset;
        avail_out = buf_size - offset;
    }

    if (status != JXL_ENC_SUCCESS) {
        free(output);
        JxlEncoderDestroy(encoder);
        return nullptr;
    }

    written = next_out - output;

    JxlEncoderDestroy(encoder);

    return new WebXDataBuffer(output, written);
}

WebXDataBuffer * WebXJXLImageConverter::_convertMono(unsigned char * data, int width, int height, int bytesPerLine, float quality) const {

    return nullptr;
}
