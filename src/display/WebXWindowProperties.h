#ifndef WEBX_WINDOW_PROPERTIES_H
#define WEBX_WINDOW_PROPERTIES_H

#include "WebXWindow.h"

class WebXWindowProperties {
public:
    WebXWindowProperties(WebXWindow * window) :
        id((unsigned long)window->getX11Window()),
        x(window->getRectangle().x()),
        y(window->getRectangle().y()),
        width(window->getRectangle().size().width()),
        height(window->getRectangle().size().height()) {
    }
    WebXWindowProperties(const WebXWindowProperties & window) :
        id(window.id),
        x(window.x),
        y(window.y),
        width(window.width),
        height(window.height) {
    }
    virtual ~WebXWindowProperties() {}

    WebXWindowProperties & operator=(const WebXWindowProperties & window) {
        if (this != &window) {
            this->id = window.id;
            this->x = window.x;
            this->y = window.y;
            this->width = window.width;
            this->height = window.height;
        }
        return *this;
    }

    unsigned long id;
    int x;
    int y;
    int width;
    int height;
};


#endif /* WEBX_WINDOW_PROPERTIES */