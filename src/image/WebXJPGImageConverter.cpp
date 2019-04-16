#include "WebXJPGImageConverter.h"
#include "WebXImage.h"
#include <toojpeg/toojpeg.h>
#include <cstring>
#include <chrono>

WebXJPGImageConverter::WebXJPGImageConverter() {
}

WebXJPGImageConverter::~WebXJPGImageConverter() {

}

WebXImage * WebXJPGImageConverter::convert(XImage * image, bool hasAlphaChannel) const {

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    WebXDataBuffer * rawData = new WebXDataBuffer(1024);
    TooJpeg::WRITE_ONE_BYTE lambda = [](unsigned char oneByte) mutable {
        // size_t newSize = this->_rawData.size + 1;
        // size_t capacity = this->_rawData.capacity;

        // /* allocate or grow buffer */
        // while (newSize > capacity) {
        //     capacity = capacity * 2;
        // }

        // if (capacity > this->_rawData.capacity) {
        //     this->_rawData.buffer = (unsigned char *)realloc(this->_rawData.buffer, capacity);
        //     this->_rawData.capacity = capacity;
        // }

        // if (!this->_rawData.buffer) {
        //     printf("Couldn't write data\n");
        // }

        // /* copy new bytes to end of buffer */
        // this->_rawData.buffer[this->_rawData.size] = oneByte;
        // this->_rawData.size = newSize;
    };


    bool ok = TooJpeg::writeJpeg(lambda, image->data, image->width, image->height);

    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> duration = end - start;

    WebXImage * webXImage = new WebXImage(WebXImageTypeJPG, image->width, image->height, rawData, image->depth, duration.count());
    return webXImage;
}
