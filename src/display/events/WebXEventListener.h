#ifndef WEBX_EVENT_LISTENER_H
#define WEBX_EVENT_LISTENER_H

#include <X11/Xlib.h>
#include <X11/extensions/Xdamage.h>
#include <map>
#include <functional>
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
     * Filter function to wait for a ConfigureNotify event and set up a filter for damage events
     * when the configure event is received.
     * @param event The X11 event to filter.
     * @return True if the event should be processed, false if it should be ignored.
     */
    bool waitForConfigureEventFilter(const XEvent * event) {
        if (event->type == ConfigureNotify) {
            XConfigureEvent * configureEvent = (XConfigureEvent *)event;
            XID filterWindow = configureEvent->window;
            unsigned long filterSerial = configureEvent->serial;

            /**
             * Set up the new filter function to handle damage events for the specified window
             * and serial number.
             */
            this->_filterFunction = [this, filterWindow, filterSerial](const XEvent * event) {
                return this->damageEventFilter(event, filterWindow, filterSerial);
            };
        }
        return true;
    }

    /**
     * Filter function to handle damage events and ignore them if they match the specified
     * window and serial number. These damage events occur immediately after the ConfigureNotify
     * and have the same serial number. These Damage events have a "more" flag that indicates if
     * there are more events to come. On the last one we reset the filter function to wait
     * for the next ConfigureNotify event.
     * @param event The X11 event to filter.
     * @param filterWindow The window to filter events for.
     * @param filterSerial The serial number to filter events for.
     * @return True if the event should be processed, false if it should be ignored.
     */
    bool damageEventFilter(const XEvent * event, const XID filterWindow, const unsigned long filterSerial) {
        if (event->type == this->_damageEventBase + XDamageNotify) {
            XDamageNotifyEvent * damageEvent = (XDamageNotifyEvent *) event;
            if (damageEvent->drawable == filterWindow && damageEvent->serial == filterSerial) {
                spdlog::trace("Ignoring damage event for window 0x{:08x} with serial {:d}", damageEvent->drawable, damageEvent->serial);
    
                /**
                 * When the last damage event is received, reset the filter function to wait for
                 * the next ConfigureNotify event.
                 */
                if (!damageEvent->more) {
                    this->_filterFunction = [this](const XEvent * event) {
                        return this->waitForConfigureEventFilter(event);
                    };
                }
                return false;
            }    
        }
        return true;
    }

private:
    Display * _x11Display;
    Window _rootWindow;
    std::map<WebXEventType, std::function<void(const WebXEvent &)>> _eventHandlers;
    WebXDamageOverride * _damageOverride;

    int _damageEventBase;
    int _damageErrorBase;
    int _xfixesEventBase;
    int _xfixesErrorBase;

    std::function<bool(const XEvent * event)> _defaultFilterFunction;
    std::function<bool(const XEvent * event)> _filterFunction;
};

#endif /* WEBX_EVENT_LISTENER_H */