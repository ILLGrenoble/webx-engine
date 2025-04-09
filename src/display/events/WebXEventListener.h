#ifndef WEBX_EVENT_LISTENER_H
#define WEBX_EVENT_LISTENER_H

#include <X11/Xlib.h>
#include <X11/extensions/Xdamage.h>
#include <map>
#include <functional>
#include <chrono>
#include "WebXEvent.h"
#include <models/WebXSettings.h>

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
    WebXEventListener(const WebXSettings & settings, Display * display, Window rootWindow);

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

    /**
     * Filter function to handle damage events and ignore them if they match
     * window and serial numbers from configuration notify events. 
     * These damage events occur immediately after the ConfigureNotify
     * and have the same serial number. These Damage events have a "more" flag that indicates if
     * there are more events to come. On the last one we remove the associated filter.
     * To avoid potential leaks, the damage filters are remove from the list if they are older than 1 second
     * @param event The X11 event to filter.
     * @return true if the event should be processed, false if it should be ignored.
     */
    bool filter(const XEvent * event);

private:
    class WebXDamageFilter {
    public:
        WebXDamageFilter(const XID window, const unsigned long serial) :
            timestamp(std::chrono::high_resolution_clock::now()),
            window(window), 
            serial(serial) {}
            
        const std::chrono::high_resolution_clock::time_point timestamp;
        const XID window;
        const unsigned long serial;
    };

private:
    Display * _x11Display;
    Window _rootWindow;
    std::map<WebXEventType, std::function<void(const WebXEvent &)>> _eventHandlers;
    WebXDamageOverride * _damageOverride;

    int _damageEventBase;
    int _damageErrorBase;
    int _xfixesEventBase;
    int _xfixesErrorBase;

    std::vector<std::unique_ptr<WebXDamageFilter>> _damageFilters;
    std::function<bool(const XEvent * event)> _filterFunction;
};

#endif /* WEBX_EVENT_LISTENER_H */