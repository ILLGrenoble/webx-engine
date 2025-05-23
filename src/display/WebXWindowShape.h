#ifndef WEBX_WINDOW_SHAPE_H
#define WEBX_WINDOW_SHAPE_H

#include <models/WebXQuality.h>
#include <memory>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

class WebXImageConverter;
class WebXImage;

class WebXWindowShape {

public:
    WebXWindowShape(Display * display, Window x11Window, int width, int height) :
        _display(display),
        _x11Window(x11Window),
        _width(width),
        _height(height),
        _isBuilt(false),
        _shapeMask(nullptr) {
    }
    virtual ~WebXWindowShape() {
    }

    void update(int width, int height, WebXImageConverter * imageConverter, const WebXQuality & quality);

    bool isActive() const {
        return this->_shapeMask != nullptr;
    }

    std::shared_ptr<WebXImage> getShapeMask() const {
        return this->_shapeMask;
    }

private:
    void create(WebXImageConverter * imageConverter, const WebXQuality & quality);

private:
    Display * _display;
    Window _x11Window;
    int _width;
    int _height;

    std::shared_ptr<WebXImage> _shapeMask;

    bool _isBuilt;
};


#endif /* WEBX_WINDOW_SHAPE_H */