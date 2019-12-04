#ifndef WEBX_EVENT_LISTENER_H
#define WEBX_EVENT_LISTENER_H

#include <X11/Xlib.h>
#include <map>
#include <functional>
#include "WebXEvent.h"

class WebXWindow;

class WebXEventListener {
public:
    WebXEventListener(Display * display, WebXWindow * rootWindow);
    virtual ~WebXEventListener();

    void flushQueuedEvents();

    void addEventHandler(WebXEventType eventType, std::function<void(const WebXEvent &)> handler);
    void removeEventHandler(WebXEventType eventType);

    bool getDamageAvailable() const {
        return this->_damageAvailable;
    }

    bool getXFixesAvailable() const {
        return this->_xfixesAvailable;
    }

private:
    void handleEvent(const WebXEvent & event);

private:
    Display * _x11Display;
    WebXWindow * _rootWindow;
    std::map<WebXEventType, std::function<void(const WebXEvent &)>> _eventHandlers;

    int _damageEventBase;
    int _damageErrorBase;
    bool _damageAvailable;
    int _xfixesEventBase;
    int _xfixesErrorBase;
    bool _xfixesAvailable;
};


#endif /* WEBX_EVENT_LISTENER_H */