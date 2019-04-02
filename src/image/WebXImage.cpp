#include "WebXImage.h"

WebXImage::WebXImage(WebXImageType type, unsigned int width, unsigned int height, unsigned int depth) :
    _type(type),
    _width(width),
    _height(height),
    _depth(depth) {
}

WebXImage::~WebXImage() {
}