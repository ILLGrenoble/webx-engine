#ifndef WEBX_MANAGER_H
#define WEBX_MANAGER_H

#include <X11/Xlib.h>
#include <string>
#include <vector>
#include <functional>
#include <events/WebXEvent.h>
#include "WebXDisplayEventType.h"

class WebXWindow;
class WebXDisplay;
class WebXEventListener;

class WebXManager {

public:
    static int IO_ERROR_HANDLER(Display *disp);

    WebXManager(const std::string & keyboardLayout = "");
    virtual ~WebXManager();


    WebXDisplay * getDisplay() const {
        return this->_display;
    }

    void handlePendingEvents();

    void setDisplayEventHandler(std::function<void(WebXDisplayEventType eventType)> handler) {
        this->_onDisplayEvent = handler;
    }

private:
    void init(const std::string & keyboardLayout = "");

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

    void sendDisplayEvent(WebXDisplayEventType eventType) {
        if (this->_onDisplayEvent) {
            this->_onDisplayEvent(eventType);
        }
    }

private:
    Display * _x11Display;
    WebXDisplay * _display;
    WebXEventListener * _eventListener;
    bool _displayRequiresUpdate;

    std::function<void(WebXDisplayEventType eventType)> _onDisplayEvent;
};


#endif /* WEBX_MANAGER_H */