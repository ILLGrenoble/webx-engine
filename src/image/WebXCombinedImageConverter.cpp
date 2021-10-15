#include "WebXCombinedImageConverter.h"


WebXCombinedImageConverter::WebXCombinedImageConverter() {
}

WebXCombinedImageConverter::~WebXCombinedImageConverter() {
}

WebXImage * WebXCombinedImageConverter::convert(XImage * image, bool hasAlphaChannel) const {
    // if (image->depth == 32) {
    //     return this->_pngImageConverter.convert(image, hasAlphaChannel);

    // } else {
        return this->_jpgImageConverter.convert(image, hasAlphaChannel);
    // }
}

WebXImage * WebXCombinedImageConverter::convert(unsigned char * data, int width, int height, int bytesPerLine, int imageDepth) const {
    // if (imageDepth == 32) {
    //     return this->_pngImageConverter.convert(data, width, height, bytesPerLine, imageDepth);

    // } else {
        return this->_jpgImageConverter.convert(data, width, height, bytesPerLine, imageDepth);
    // }
}

