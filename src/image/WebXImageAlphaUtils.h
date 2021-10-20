#ifndef WEBX_IMAGE_ALPHA_UTILS_H
#define WEBX_IMAGE_ALPHA_UTILS_H

#include <X11/Xlib.h>
#include <utils/WebXRectangle.h>

class WebXImageAlphaUtils {

public:
    static bool convert(XImage * image, WebXRectangle * mainWindowRectangle, WebXRectangle * subWindowRectangle, WebXRectangle * imageRectangle);
    static bool hasAlpha(XImage * image, const WebXRectangle & rectangle);

private:

};


#endif /* WEBX_IMAGE_ALPHA_UTILS_H */