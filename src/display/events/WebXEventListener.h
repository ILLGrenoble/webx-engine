#ifndef WEBX_EVENT_LISTENER_H
#define WEBX_EVENT_LISTENER_H

#include <X11/Xlib.h>
#include <map>
#include <functional>
#include "WebXEvent.h"

class WebXDamageOverride;

class WebXEventListener {
public:
    WebXEventListener(Display * display, Window rootWindow);
    virtual ~WebXEventListener();

    void flushQueuedEvents();

    void addEventHandler(WebXEventType eventType, std::function<void(const WebXEvent &)> handler);
    void removeEventHandler(WebXEventType eventType);

private:
    void handleEvent(const WebXEvent & event);

private:
    Display * _x11Display;
    Window _rootWindow;
    std::map<WebXEventType, std::function<void(const WebXEvent &)>> _eventHandlers;
    WebXDamageOverride * _damageOverride;

    int _damageEventBase;
    int _damageErrorBase;
    int _xfixesEventBase;
    int _xfixesErrorBase;
};


#endif /* WEBX_EVENT_LISTENER_H */