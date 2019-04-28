#include "WebXMouseCursorImage.h"

WebXMouseCursorImage::~WebXMouseCursorImage() {
    if (this->_rawData) {
        delete this->_rawData;
        this->_rawData = NULL;
    }
}

WebXMouseCursorImage::WebXMouseCursorImage(unsigned int width, unsigned int height, WebXDataBuffer * rawData, double encodingTimeUs) :
    _width(width),
    _height(height),
    _rawData(rawData),
    _encodingTimeUs(encodingTimeUs) {
}

bool WebXMouseCursorImage::save(const std::string &fileName) {
    if (this->getRawData() != NULL) {
        FILE * fp = fopen(fileName.c_str(), "wb");

        if (!fp) {
            return false;
        }

        fwrite(this->getRawData(), this->getRawDataSize(), 1, fp);
        fclose(fp);

        return true;
    } else {
        return false;
    }
}
