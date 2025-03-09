#include "WebXDamageOverride.h"
#include <X11/extensions/Xdamage.h>
#include <X11/Xlibint.h>

WireToEventType WebXDamageOverride::ORIGINAL_WIRE_TO_EVENT_HANDLER = 0;
Window WebXDamageOverride:: WINDOW_TO_IGNORE = 0;

int WebXDamageOverride::WIRE_TO_EVENT_WRAPPER(Display * dpy, XEvent * event, xEvent *wire) {
    
    // Verify that 
    if (ORIGINAL_WIRE_TO_EVENT_HANDLER) {
        Bool originalReturnValue = ORIGINAL_WIRE_TO_EVENT_HANDLER(dpy, event, wire);
        if (originalReturnValue == True && WINDOW_TO_IGNORE != 0) {
            XDamageNotifyEvent * damageEvent = (XDamageNotifyEvent *) event;

            return (damageEvent->drawable != WINDOW_TO_IGNORE);
      
        } else {
            return originalReturnValue;
        }
    }

    return False;
}


void WebXDamageOverride::enable() {
    ORIGINAL_WIRE_TO_EVENT_HANDLER = XESetWireToEvent(this->_display, this->_eventNumber, WIRE_TO_EVENT_WRAPPER);
}

void WebXDamageOverride::disable() {
    if (ORIGINAL_WIRE_TO_EVENT_HANDLER != 0) {
        XESetWireToEvent(this->_display, this->_eventNumber, ORIGINAL_WIRE_TO_EVENT_HANDLER);
        ORIGINAL_WIRE_TO_EVENT_HANDLER = 0;
    }
}
