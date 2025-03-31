#ifndef WEBX_EVENT_LISTENER_H
#define WEBX_EVENT_LISTENER_H

#include <X11/Xlib.h>
#include <map>
#include <functional>
#include "WebXEvent.h"

class WebXDamageOverride;

/**
 * The WebXEventListener class is responsible for listening to and handling X11 events.
 * It allows registering custom event handlers for specific event types and manages
 * the dispatching of events to the appropriate handlers.
 */
class WebXEventListener {
public:
    /**
     * Constructor to initialize the event listener with the X11 display and root window.
     * @param display Pointer to the X11 display.
     * @param rootWindow The root window of the X11 display.
     */
    WebXEventListener(Display * display, Window rootWindow);

    /**
     * Destructor to clean up resources used by the event listener.
     */
    virtual ~WebXEventListener();

    /**
     * Flushes any queued events that need to be processed.
     */
    void flushQueuedEvents();

    /**
     * Adds an event handler for a specific event type.
     * @param eventType The type of event to handle.
     * @param handler A function to handle the event.
     */
    void addEventHandler(WebXEventType eventType, std::function<void(const WebXEvent &)> handler);

    /**
     * Removes the event handler for a specific event type.
     * @param eventType The type of event whose handler should be removed.
     */
    void removeEventHandler(WebXEventType eventType);

private:
    /**
     * Internal method to handle an incoming event and dispatch it to the appropriate handler.
     * @param event The event to handle.
     */
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