#include "WebXImage.h"

WebXImage::WebXImage(WebXImageType type, unsigned int width, unsigned int height, unsigned int depth, double encodingTimeUs) :
    _type(type),
    _width(width),
    _height(height),
    _depth(depth),
    _encodingTimeUs(encodingTimeUs) {
}

WebXImage::~WebXImage() {
}