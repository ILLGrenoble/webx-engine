#ifndef WEBX_MANAGER_H
#define WEBX_MANAGER_H

#include <X11/Xlib.h>
#include <vector>
#include <events/WebXEvent.h>

class WebXWindow;
class WebXDisplay;
class WebXEventListener;

class WebXManager {

public:
    static int IO_ERROR_HANDLER(Display *disp);

    WebXManager();
    virtual ~WebXManager();

    void init();

    WebXDisplay * getDisplay() const {
        return this->_display;
    }

    void flushEventListener();

private:
    void handleWindowCreateEvent(const WebXEvent & event);
    void handleWindowDestroyEvent(const WebXEvent & event);
    void handleWindowMapEvent(const WebXEvent & event);
    void handleWindowUnmapEvent(const WebXEvent & event);
    void handleWindowReparentEvent(const WebXEvent & event);
    void handleWindowConfigureEvent(const WebXEvent & event);
    void handleWindowGravityEvent(const WebXEvent & event);
    void handleWindowCirculateEvent(const WebXEvent & event);
    void handleWindowDamageEvent(const WebXEvent & event);
    void handleMouseCursorEvent(const WebXEvent & event);

    void updateDisplay();

private:
    Display * _x11Display;
    WebXDisplay * _display;
    WebXEventListener * _eventListener;
};


#endif /* WEBX_MANAGER_H */