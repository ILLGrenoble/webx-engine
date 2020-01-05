#include "WebXCombinedImageConverter.h"
#include "WebXPNGImageConverter.h"
#include "WebXJPGImageConverter.h"

WebXCombinedImageConverter::WebXCombinedImageConverter() :
    _pngImageConverter(new WebXPNGImageConverter()),
    _jpgImageConverter(new WebXJPGImageConverter()) {
}

WebXCombinedImageConverter::~WebXCombinedImageConverter() {
    if (_pngImageConverter) {
        delete _pngImageConverter;
    }
    if (_jpgImageConverter) {
        delete _jpgImageConverter;
    }
}

WebXImage * WebXCombinedImageConverter::convert(XImage * image, bool hasAlphaChannel) const {
    if (image->depth == 32) {
        return this->_pngImageConverter->convert(image, hasAlphaChannel);

    } else {
        return this->_jpgImageConverter->convert(image, hasAlphaChannel);
    }
}

WebXImage * WebXCombinedImageConverter::convert(const unsigned char * data, int width, int height, int bytesPerLine, int imageDepth) const {
    if (imageDepth == 32) {
        return this->_pngImageConverter->convert(data, width, height, bytesPerLine, imageDepth);

    } else {
        return this->_jpgImageConverter->convert(data, width, height, bytesPerLine, imageDepth);
    }
}

