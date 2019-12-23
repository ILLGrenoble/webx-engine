#include "WebXMouseCursor.h"
#include "WebXMouseCursorImageConverter.h"

WebXMouseCursor::WebXMouseCursor(XFixesCursorImage * cursor) :
    _cursor(cursor),
    _converter(new WebXMouseCursorImageConverter()) {
}

WebXMouseCursor::~WebXMouseCursor() {
    if (_cursor != NULL) {
        XFree(_cursor);
        _cursor = NULL;
    }
    delete _converter;
}

std::shared_ptr<WebXMouseCursorImage> WebXMouseCursor::getImage() {
    auto image = _converter->convert(_cursor);
    return std::shared_ptr<WebXMouseCursorImage>(image);
}


