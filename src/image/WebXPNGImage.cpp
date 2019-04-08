#include "WebXPNGImage.h"

WebXPNGImage::WebXPNGImage(unsigned int width, unsigned int height, unsigned char * rawData, size_t length, unsigned int depth) :
    WebXImage(WebXImageTypePNG, width, height, depth),
    _rawData(rawData),
    _length(length) {
}

WebXPNGImage::~WebXPNGImage() {
    if (this->_rawData) {
        free (this->_rawData);
        this->_rawData = NULL;
    }
}

bool WebXPNGImage::save(const std::string & fileName) {
    FILE * fp = fopen(fileName.c_str(), "wb");

    if (!fp) {
        return false;
    }

    fwrite(this->_rawData, this->_length, 1, fp);

    fclose(fp);

    return true;
}
