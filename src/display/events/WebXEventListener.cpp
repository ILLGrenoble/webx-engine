#include "WebXEventListener.h"
#include "WebXEvent.h"
#include "WebXDamageOverride.h"
#include <display/WebXWindow.h>
#include <models/WebXSettings.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xdamage.h>
#include <X11/extensions/Xrender.h>
#include <X11/Xutil.h>
#include <spdlog/spdlog.h>


WebXEventListener::WebXEventListener(const WebXSettings & settings, Display * display, Window rootWindow) :
    _x11Display(display),
    _rootWindow(rootWindow),
    _damageEventBase(0),
    _damageErrorBase(0),
    _xfixesEventBase(0),
    _xfixesErrorBase(0) {

    XSelectInput(this->_x11Display, this->_rootWindow, SubstructureNotifyMask);

    if (!XDamageQueryExtension(this->_x11Display, &this->_damageEventBase, &this->_damageErrorBase)) {
        spdlog::error("No damage extension");
        exit(1);
    }
    if (XFixesQueryExtension(this->_x11Display, &this->_xfixesEventBase, &this->_xfixesErrorBase)) {
        XFixesSelectCursorInput(this->_x11Display,  this->_rootWindow, XFixesDisplayCursorNotifyMask);
    } else {
        spdlog::error("No xfixes extension");
        exit(1);
    }

    if (settings.event.filterDamageAfterConfigureNotify) {
        // Set up the filter function to filter damage events after configure notify
        this->_filterFunction = [this](const XEvent * event) { return this->filter(event); };

    } else {
        // Set up the filter function to keep all events
        this->_filterFunction = [](const XEvent * event) { return true; };
    }

    // Create and enable the damage overrider
    this->_damageOverride = new WebXDamageOverride(display, this->_damageEventBase);
    this->_damageOverride->enable();
}

WebXEventListener::~WebXEventListener() {
    if (this->_damageOverride) {
        this->_damageOverride->disable();
        delete this->_damageOverride;
        this->_damageOverride = 0;
    }
}

void WebXEventListener::flushQueuedEvents() {
    XEvent x11Event;

    XFlush(this->_x11Display);
    int qLength = QLength(this->_x11Display);

    // Loop through all events in the queue. Filter them using the filter function (removes excesss damage events created when the ConfigureNotify event is received)
    for (int i = 0; i < qLength; i++) {
        XNextEvent(this->_x11Display, &x11Event);
        if (this->_filterFunction(&x11Event)) {
            WebXEvent event(x11Event, this->_damageEventBase, this->_xfixesEventBase);
            this->handleEvent(event);
        }
    }

    // Remove old damage filters (older than 1 second)
    auto now = std::chrono::high_resolution_clock::now();
    this->_damageFilters.erase(std::remove_if(this->_damageFilters.begin(), this->_damageFilters.end(),
        [&now](const std::unique_ptr<WebXDamageFilter> & damageFilter) {
            return (now - damageFilter->timestamp) > std::chrono::seconds(1);
        }), this->_damageFilters.end());
}

bool WebXEventListener::filter(const XEvent * event) {
    if (event->type == this->_damageEventBase + XDamageNotify) {
        // Handle damage event
        XDamageNotifyEvent * damageEvent = (XDamageNotifyEvent *) event;
        auto it = std::find_if(this->_damageFilters.begin(), this->_damageFilters.end(), 
        [&damageEvent](const std::unique_ptr<WebXDamageFilter> & damageFilter) {
            return damageFilter->window == damageEvent->drawable && damageFilter->serial == damageEvent->serial;
        });

        // Damage event is not associated to a configure notify
        if (it == this->_damageFilters.end()) {
            return true;
        }

        // Remove the filter if there are no more damage events
        if (!damageEvent->more) {
            this->_damageFilters.erase(it);
        }

        return false;

    } else if (event->type == ConfigureNotify) {
        // Handle configure event
        XConfigureEvent * configureEvent = (XConfigureEvent *) event;
        this->_damageFilters.push_back(std::unique_ptr<WebXDamageFilter>(new WebXDamageFilter(configureEvent->window, configureEvent->serial)));

        return true;
    
    } else {
        // All other events
        return true;
    }
}

void WebXEventListener::handleEvent(const WebXEvent & event) {
    std::map<WebXEventType, std::function<void(const WebXEvent &)>>::iterator it = this->_eventHandlers.find(event.getType());

    if (it != this->_eventHandlers.end()) {
        std::function<void(const WebXEvent &)> handler = it->second;

        handler(event);
    }
}

void WebXEventListener::addEventHandler(WebXEventType eventType, std::function<void(const WebXEvent &)> handler) {
    this->_eventHandlers[eventType] = handler;
}

void WebXEventListener::removeEventHandler(WebXEventType eventType) {
    std::map<WebXEventType, std::function<void(const WebXEvent &)>>::iterator it = this->_eventHandlers.find(eventType);
    if (it != this->_eventHandlers.end()) {
        this->_eventHandlers.erase(it);
    }
}
