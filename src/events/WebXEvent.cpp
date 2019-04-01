#include "WebXEvent.h"
#include <stdio.h>

WebXEvent::WebXEvent(const XEvent & xEvent) :
    _xEvent(xEvent) {

    this->convert();
}

WebXEvent::~WebXEvent() {
}

void WebXEvent::convert() {
   switch (this->_xEvent.type) {
    case CreateNotify:
        this->_type = WebXEventType::Create;
        this->_x11Window = this->_xEvent.xcreatewindow.window;
        this->_parent = this->_xEvent.xcreatewindow.parent;
        this->_x = this->_xEvent.xconfigure.x;
        this->_y = this->_xEvent.xconfigure.y;
        this->_width = this->_xEvent.xconfigure.width;
        this->_height = this->_xEvent.xconfigure.height;
        break;

    case DestroyNotify:
        this->_type = WebXEventType::Destroy;
        this->_x11Window = this->_xEvent.xdestroywindow.window;
        break;

    case MapNotify:
        this->_type = WebXEventType::Map;
        this->_x11Window = this->_xEvent.xmap.window;
        break;

    case UnmapNotify:
        this->_type = WebXEventType::Unmap;
        this->_x11Window = this->_xEvent.xunmap.window;
        break;

    case ReparentNotify:
        this->_type = WebXEventType::Reparent;
        this->_x11Window = this->_xEvent.xreparent.window;
        this->_parent = this->_xEvent.xreparent.parent;
        break;

    case ConfigureNotify:
        this->_type = WebXEventType::Configure;
        this->_x11Window = this->_xEvent.xconfigure.window;
        this->_x = this->_xEvent.xconfigure.x;
        this->_y = this->_xEvent.xconfigure.y;
        this->_width = this->_xEvent.xconfigure.width;
        this->_height = this->_xEvent.xconfigure.height;
        break;

    case GravityNotify:
        this->_type = WebXEventType::Gravity;
        this->_x11Window = this->_xEvent.xgravity.window;
        break;

    case CirculateNotify:
        this->_type = WebXEventType::Circulate;
        this->_x11Window = this->_xEvent.xcirculate.window;
        break;

    default:
        this->_type = WebXEventType::Other;
        this->_x11Window = 0;
    }
}
