#include "WebXMouseCursor.h"

WebXMouseCursor::WebXMouseCursor(std::shared_ptr<WebXImage> image, int xhot, int yhot) :
    _image(image),
    _xhot(xhot),
    _yhot(yhot) {
}

WebXMouseCursor::~WebXMouseCursor() {
}

