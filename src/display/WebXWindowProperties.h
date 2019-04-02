#ifndef WEBX_WINDOW_PROPERTIES_H
#define WEBX_WINDOW_PROPERTIES_H

#include <utils/WebXRectangle.h>
#include "WebXWindow.h"

class WebXWindowProperties {
public:
    WebXWindowProperties(const WebXWindow * window) :
        id((unsigned long)window->getX11Window()),
        rectangle(window->getRectangle()) {
    }
    WebXWindowProperties(const WebXWindowProperties & window) :
        id(window.id),
        rectangle(window.rectangle) {
    }
    virtual ~WebXWindowProperties() {}

    WebXWindowProperties & operator=(const WebXWindowProperties & window) {
        if (this != &window) {
            this->id = window.id;
            this->rectangle = window.rectangle;
        }
        return *this;
    }

    unsigned long id;
    WebXRectangle rectangle;
};


#endif /* WEBX_WINDOW_PROPERTIES */