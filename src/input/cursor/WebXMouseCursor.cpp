#include "WebXMouseCursor.h"

WebXMouseCursor::WebXMouseCursor(const std::string & name, unsigned long serial, std::shared_ptr<WebXImage> image, int xhot, int yhot) :
    _name(name),
    _serial(serial),
    _image(image),
    _xhot(xhot),
    _yhot(yhot) {
}

WebXMouseCursor::~WebXMouseCursor() {
}

