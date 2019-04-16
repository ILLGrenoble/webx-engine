#include "WebXJPGImage.h"

WebXJPGImage::WebXJPGImage(unsigned int width, unsigned int height, unsigned char * rawData, size_t length, unsigned int depth, double encodingTimeUs) :
    WebXImage(WebXImageTypePNG, width, height, depth, encodingTimeUs),
    _rawData(rawData),
    _length(length) {
}

WebXJPGImage::~WebXJPGImage() {
    if (this->_rawData) {
        free (this->_rawData);
        this->_rawData = NULL;
    }
}

bool WebXJPGImage::save(const std::string & fileName) {
    FILE * fp = fopen(fileName.c_str(), "wb");

    if (!fp) {
        return false;
    }

    fwrite(this->_rawData, this->_length, 1, fp);

    fclose(fp);

    return true;
}
