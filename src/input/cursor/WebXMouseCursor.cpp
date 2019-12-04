#include <input/cursor/WebXMouseCursorImageConverter.h>
#include "WebXMouseCursor.h"

WebXMouseCursor::~WebXMouseCursor() {
    if (_cursor != NULL) {
        XFree(_cursor);
    }
    delete _converter;
}

WebXMouseCursor::WebXMouseCursor(XFixesCursorImage * cursor) :
        _cursor(cursor),
        _converter(new WebXMouseCursorImageConverter()) {
}

std::shared_ptr<WebXMouseCursorImage> WebXMouseCursor::getImage() {
    auto image = _converter->convert(_cursor);
    return std::shared_ptr<WebXMouseCursorImage>(image);
}


