#include "WebXEvent.h"
#include <X11/extensions/Xdamage.h>
#include <stdio.h>
#include <spdlog/spdlog.h>

WebXEvent::WebXEvent(const XEvent & xEvent, int damageEventBase, int fixesEventBase) :
    _xEvent(xEvent),
    _damageEventBase(damageEventBase),
    _fixesEventBase(fixesEventBase) {

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

    case SelectionNotify:
        this->_type = WebXEventType::ClipboardNotify;

        break;

    case SelectionRequest:
        this->_type = WebXEventType::ClipboardContentRequest;

        break;

    default:
        if (this->_xEvent.type == this->_damageEventBase +  XDamageNotify) {
            this->_type = WebXEventType::Damaged;
            XDamageNotifyEvent * damageEvent = (XDamageNotifyEvent *) &this->_xEvent;
            this->_x11Window = damageEvent->drawable;
            this->_x = damageEvent->area.x;
            this->_y = damageEvent->area.y;
            this->_width = damageEvent->area.width;
            this->_height = damageEvent->area.height;
            // printf("Damage event on window 0x%0lx area = [%d %d %d %d], geometry = [%d %d %d %d]\n", this->_x11Window, 
            //     damageEvent->area.x, damageEvent->area.y, damageEvent->area.width, damageEvent->area.height,
            //     damageEvent->geometry.x, damageEvent->geometry.y, damageEvent->geometry.width, damageEvent->geometry.height);
        } else if (this->_xEvent.type == this->_fixesEventBase + XFixesCursorNotify) {
            // XFixesCursorNotifyEvent?
            // TODO cast this to a XFixesCursorNotifyEvent. Although this would pollute this class by adding cursor attributes...
            this->_type = WebXEventType::MouseCursor;
        } else {
            this->_type = WebXEventType::Other;
            this->_x11Window = 0;
        }
    }
}
