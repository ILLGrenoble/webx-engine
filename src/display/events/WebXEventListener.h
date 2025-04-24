#ifndef WEBX_EVENT_LISTENER_H
#define WEBX_EVENT_LISTENER_H

#include <X11/Xlib.h>
#include <map>
#include <functional>
#include <models/WebXSettings.h>

#include "WebXMapEvent.h"
#include "WebXUnmapEvent.h"
#include "WebXReparentEvent.h"
#include "WebXConfigureEvent.h"
#include "WebXSelectionEvent.h"
#include "WebXSelectionRequestEvent.h"
#include "WebXDamageEvent.h"
#include "WebXCursorEvent.h"

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
     * Sets the event handler for the map event
     * @param handler The handler for the map event
     */
    void setMapEventHandler(std::function<void(const WebXMapEvent &)> handler) {
        this->_mapEventHandler = handler;
    }

    /**
     * Sets the event handler for the unmap event
     * @param handler The handler for the unmap event
     */
    void setUnmapEventHandler(std::function<void(const WebXUnmapEvent &)> handler) {
        this->_unmapEventHandler = handler;
    }

    /**
     * Sets the event handler for the reparent event
     * @param handler The handler for the reparent event
     */
    void setReparentEventHandler(std::function<void(const WebXReparentEvent &)> handler) {
        this->_reparentEventHandler = handler;
    }

    /**
     * Sets the event handler for the configuration event
     * @param handler The handler for the configuration event
     */
    void setConfigureEventHandler(std::function<void(const WebXConfigureEvent &)> handler) {
        this->_configureEventHandler = handler;
    }

    /**
     * Sets the event handler for the selection event
     * @param handler The handler for the selection event
     */
    void setSelectionEventHandler(std::function<void(const WebXSelectionEvent &)> handler) {
        this->_selectionEventHandler = handler;
    }

    /**
     * Sets the event handler for the selection request event
     * @param handler The handler for the selection request event
     */
    void setSelectionRequestEventHandler(std::function<void(const WebXSelectionRequestEvent &)> handler) {
        this->_selectionRequestEventHandler = handler;
    }

    /**
     * Sets the event handler for the damage event
     * @param handler The handler for the damage event
     */
    void setDamageEventHandler(std::function<void(const WebXDamageEvent &)> handler) {
        this->_damageEventHandler = handler;
    }

    /**
     * Sets the event handler for the cursor event
     * @param handler The handler for the cursor event
     */
    void setCursorEventHandler(std::function<void(const WebXCursorEvent &)> handler) {
        this->_cursorEventHandler = handler;
    }

private:
    /**
     * Handles all the XEvents and converts the to WebX events
     * @param event the XEvent to be handled
     */
    void handleXEvent(const XEvent * event);

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

    std::function<void(const WebXMapEvent &)> _mapEventHandler;
    std::function<void(const WebXUnmapEvent &)> _unmapEventHandler;
    std::function<void(const WebXReparentEvent &)> _reparentEventHandler;
    std::function<void(const WebXConfigureEvent &)> _configureEventHandler;
    std::function<void(const WebXSelectionEvent &)> _selectionEventHandler;
    std::function<void(const WebXSelectionRequestEvent &)> _selectionRequestEventHandler;
    std::function<void(const WebXDamageEvent &)> _damageEventHandler;
    std::function<void(const WebXCursorEvent &)> _cursorEventHandler;

    WebXDamageOverride * _damageOverride;

    int _damageEventBase;
    int _damageErrorBase;
    int _xfixesEventBase;
    int _xfixesErrorBase;

    std::vector<std::unique_ptr<WebXDamageFilter>> _damageFilters;
    std::function<bool(const XEvent * event)> _filterFunction;
};

#endif /* WEBX_EVENT_LISTENER_H */