#ifndef WEBX_MOUSE_CURSOR_H
#define WEBX_MOUSE_CURSOR_H

#include <X11/extensions/Xfixes.h>
#include <memory>

class WebXMouseCursorImage;
class WebXMouseCursorImageConverter;

class WebXMouseCursor {
public:

    WebXMouseCursor(XFixesCursorImage * cursorImage);

    ~WebXMouseCursor();

    /**
     * Ger the X11 cursor
     */
    XFixesCursorImage * getInfo() const {
        return _cursor;
    }

    /**
     * Returns a png image of the mouse cursor
     * @return  the mouse cursor image
     */
    std::shared_ptr<WebXMouseCursorImage> getImage();

    const char * getName() const {
        return _cursor->name;
    }

private:
    XFixesCursorImage * _cursor;
    WebXMouseCursorImageConverter * _converter;
};


#endif //WEBX_MOUSE_CURSOR_H
