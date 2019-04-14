#ifndef WEBX_IMAGE_ALPHA_CONVERTER_H
#define WEBX_IMAGE_ALPHA_CONVERTER_H

#include <X11/Xlib.h>
#include <utils/WebXRectangle.h>

class WebXImageAlphaConverter {

public:
    static bool convert(XImage * image, WebXRectangle * mainWindowRectangle, WebXRectangle * subWindowRectangle, WebXRectangle * imageRectangle);

private:

};


#endif /* WEBX_IMAGE_ALPHA_CONVERTER_H */