#include "WebXMouseCursor.h"

WebXMouseCursor::WebXMouseCursor(uint32_t cursorId, std::shared_ptr<WebXImage> image, int xhot, int yhot) :
    _id(cursorId),
    _image(image),
    _xhot(xhot),
    _yhot(yhot) {
}

WebXMouseCursor::~WebXMouseCursor() {
}

