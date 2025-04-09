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
        // Set up the filter function to wait for a ConfigureNotify event
        this->_defaultFilterFunction = [this](const XEvent * event) { return this->waitForConfigureEventFilter(event);};

    } else {
        // Set up the filter function to keep all events
        this->_defaultFilterFunction = [](const XEvent * event) { return true; };
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
    
    // Initialise the filter function as the default
    this->_filterFunction = this->_defaultFilterFunction;

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
