#ifndef WEBX_MANAGER_H
#define WEBX_MANAGER_H

#include <X11/Xlib.h>
#include <vector>
#include <events/WebXEvent.h>

class WebXWindow;
class WebXDisplay;
class WebXEventListener;
class WebXController;

class WebXManager {
private:
    WebXManager();
    virtual ~WebXManager();

public:
    static int ERROR_HANDLER(Display *disp, XErrorEvent *err);

    static WebXManager * instance();
    void init();
    static void shutdown();

    WebXDisplay * getDisplay() const {
        return this->_display;
    }

    WebXEventListener * getEventListener() const {
        return this->_eventListener;
    }

    WebXController * getController() const {
        return this->_controller;
    }

    void pauseEventListener();
    void resumeEventListener();

private:
    void handleWindowCreateEvent(const WebXEvent & event);
    void handleWindowDestroyEvent(const WebXEvent & event);
    void handleWindowMapEvent(const WebXEvent & event);
    void handleWindowUnmapEvent(const WebXEvent & event);
    void handleWindowReparentEvent(const WebXEvent & event);
    void handleWindowConfigureEvent(const WebXEvent & event);
    void handleWindowGravityEvent(const WebXEvent & event);
    void handleWindowCirculateEvent(const WebXEvent & event);
 
    void updateDisplay();

private:
    static WebXManager * _instance;

    Display * _x11Display;
    WebXDisplay * _display;
    WebXEventListener * _eventListener;
    WebXController * _controller;
};


#endif /* WEBX_MANAGER_H */