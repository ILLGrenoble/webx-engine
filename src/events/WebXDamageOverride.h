#ifndef WEBX_DAMAGE_OVERRIDE_H
#define WEBX_DAMAGE_OVERRIDE_H

#include <X11/Xlib.h>
#include <X11/Xproto.h>

typedef int (* WireToEventType) (Display * display, XEvent * event, xEvent *	wire);

/**
 * The WebXDamageOverride contains a static function to override the default function set to XESetWireToEvent 
 * to handle damage events.
 * Noticed in ubuntu 20.04, a chrome browser can close the connection to the Xserver when doing XGetImage 
 * while XDamage is enabled. We've hacked the code to remove the XDamage events from a window during the call
 * to XGetImage and re-enable it after the call. However, the action of adding the XDamage listener to a window
 * generates a large number of XDamage events, even if the window content has not changed.
 * This function allows XDamage to filter out events for a particular window.
 */
class WebXDamageOverride {
public:
    WebXDamageOverride(Display * display, int eventNumber) :
      _display(display),
      _eventNumber(eventNumber) {
    }
    virtual ~WebXDamageOverride() {
    }

    void enable();
    void disable();

    static void setWindowToIgnore(Window windowToIgnore) {
      WINDOW_TO_IGNORE = windowToIgnore;
    }

private:
    static WireToEventType ORIGINAL_WIRE_TO_EVENT_HANDLER;
    static Window WINDOW_TO_IGNORE;

    static int WIRE_TO_EVENT_WRAPPER(Display * dpy, XEvent * event, xEvent * wire);

    Display * _display;
    int _eventNumber;
};


#endif /* WEBX_DAMAGE_OVERRIDE_H */