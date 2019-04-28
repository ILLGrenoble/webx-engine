#ifndef WEBX_MOUSE_CURSOR_IMAGE_CONVERTER_H
#define WEBX_MOUSE_CURSOR_IMAGE_CONVERTER_H

#include "input/cursor/WebXMouseCursorImage.h"
#include <png.h>
#include <stdlib.h>
#include <X11/extensions/Xfixes.h>

class WebXMouseCursorImageConverter {
public:
    WebXMouseCursorImageConverter();

    virtual ~WebXMouseCursorImageConverter();

    /**
     * Converts an x11 cursor into a png image
     * @param image the x11 cursor
     * @return the image
     */
    virtual WebXMouseCursorImage * convert(XFixesCursorImage * image) const;

private:
    static void RawDataWriter(png_struct * png, png_byte * data, png_size_t length);

};

#endif //WEBX_MOUSE_CURSOR_IMAGE_CONVERTER_H
