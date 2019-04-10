#ifndef WEBX_EVENT_H
#define WEBX_EVENT_H

#include <X11/Xlib.h>
#include <utils/WebXRectangle.h>

enum WebXEventType {
    Create = 0,
    Destroy,
    Map,
    Unmap,
    Reparent,
    Configure,
    Gravity,
    Circulate,
    Damaged,
    Other
};

class WebXEvent {
public:
    WebXEvent(const XEvent & xEvent, int damageEventBase);
    virtual ~WebXEvent();

    Window getX11Window() const {
        return this->_x11Window;
    }

    WebXEventType getType() const {
        return this->_type;
    }

    int getX() const {
        return this->_x;
    }

    int getY() const {
        return this->_y;
    }

    int getWidth() const {
        return this->_width;
    }

    int getHeight() const {
        return this->_height;
    }

    Window getParent() const {
        return this->_parent;
    }

    WebXRectangle getRectangle() const {
        return WebXRectangle(this->_x, this->_y, this->_width, this->_height);
    }

private:
    void convert();

private:
    XEvent _xEvent;
    Window _x11Window;
    WebXEventType _type;

    int _x;
    int _y;
    int _width;
    int _height;
    Window _parent;
    int _damageEventBase;
};


#endif /* WEBX_EVENT_H */