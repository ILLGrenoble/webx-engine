#ifndef WEBX_MOUSE_CURSOR_FACTORY_H
#define WEBX_MOUSE_CURSOR_FACTORY_H

#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>
#include <map>
#include <memory>
#include <mutex>
#include <image/WebXImage.h>
#include <image/WebXPNGImageConverter.h>
#include "WebXMouseCursor.h"

class WebXMouseCursorFactory {
public:

    WebXMouseCursorFactory(Display * x11Display);
    virtual ~WebXMouseCursorFactory();

    std::shared_ptr<WebXMouseCursor> createCursor();
    std::shared_ptr<WebXMouseCursor> getCursor(uint32_t cursorId);

    std::shared_ptr<WebXImage> convertCursorImage(XFixesCursorImage * cursorImage);

private:
    uint32_t calculateRawImageChecksum(XFixesCursorImage * cursorImage) const;

private:
    Display * _display;
    WebXPNGImageConverter _imageConverter;

    std::map<uint32_t, uint32_t> _checksumLookup;
    std::map<uint32_t, std::shared_ptr<WebXMouseCursor>> _catalogue;
	std::mutex _catalogueMutex;

};


#endif //WEBX_MOUSE_CURSOR_FACTORY_H
