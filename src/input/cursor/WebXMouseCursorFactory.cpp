#include "WebXMouseCursorFactory.h"
#include <crc32/Crc32.h>
#include <spdlog/spdlog.h>

WebXMouseCursorFactory::WebXMouseCursorFactory(Display * x11Display) :
    _display(x11Display) {
}

WebXMouseCursorFactory::~WebXMouseCursorFactory() {
}

std::shared_ptr<WebXMouseCursor> WebXMouseCursorFactory::createCursor() {
    std::lock_guard<std::mutex> lock(this->_catalogueMutex);

    XFixesCursorImage * cursorImage = XFixesGetCursorImage(_display);
    if (cursorImage) {
        uint32_t cursorId = this->calculateRawImageChecksum(cursorImage);
        auto cursorIt = this->_catalogue.find(cursorId);
        if (cursorIt == this->_catalogue.end()) {
            std::shared_ptr<WebXImage> image = this->convertCursorImage(cursorImage);
            if (image != NULL) {
                auto cursor =  std::shared_ptr<WebXMouseCursor>(new WebXMouseCursor(cursorId, image, (int)cursorImage->xhot, (int)cursorImage->yhot));
                this->_catalogue[cursorId] = cursor;

                spdlog::debug("Cursor catalogue contains {:d} cursors", this->_catalogue.size());

                return cursor;
            }

        } else {
            return cursorIt->second;
        }
    }

    return nullptr;
}

std::shared_ptr<WebXMouseCursor> WebXMouseCursorFactory::getCursor(uint32_t cursorId) {
    std::lock_guard<std::mutex> lock(this->_catalogueMutex);

    auto catalogueIt = this->_catalogue.find(cursorId);
    if (catalogueIt != this->_catalogue.end()) {
      return catalogueIt->second;
    }

    return nullptr;
}

uint32_t WebXMouseCursorFactory::calculateRawImageChecksum(XFixesCursorImage * cursorImage) const {
    int length = cursorImage->width * cursorImage->height * sizeof(unsigned long);
    uint32_t checksum = crc32_16bytes(cursorImage->pixels, length);
    return checksum;
}

std::shared_ptr<WebXImage> WebXMouseCursorFactory::convertCursorImage(XFixesCursorImage * cursorImage) {
    // Convert raw image data to WebXImage
    unsigned int imageByteLength = cursorImage->width * cursorImage->height * 4;
    unsigned char * imageData = (unsigned char *)malloc(imageByteLength);
    unsigned long * src = cursorImage->pixels;
    unsigned int offset = 0;
    while (offset < imageByteLength) {
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

        imageData[offset + 0] = b;
        imageData[offset + 1] = g;
        imageData[offset + 2] = r;
        imageData[offset + 3] = a;

        offset += 4;
    }

    WebXImage * image = this->_imageConverter.convert(imageData, (int)cursorImage->width, (int)cursorImage->height, (int)cursorImage->width * 4, 32, 0.95);
    free(imageData);

    return std::shared_ptr<WebXImage>(image);
}

