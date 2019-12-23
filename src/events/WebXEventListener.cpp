#include "WebXEventListener.h"
#include "WebXEvent.h"
#include <display/WebXWindow.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xdamage.h>
#include <X11/extensions/Xrender.h>
#include <X11/Xutil.h>
#include <spdlog/spdlog.h>


WebXEventListener::WebXEventListener(Display * display, WebXWindow * rootWindow) :
    _x11Display(display),
    _rootWindow(rootWindow),
    _damageEventBase(0),
    _damageErrorBase(0),
    _xfixesEventBase(0),
    _xfixesErrorBase(0) {

    XSelectInput(this->_x11Display, this->_rootWindow->getX11Window(), SubstructureNotifyMask);

    if (!XDamageQueryExtension(this->_x11Display, &this->_damageEventBase, &this->_damageErrorBase)) {
        spdlog::error("No damage extension");
        exit(1);
    }
    if (XFixesQueryExtension(this->_x11Display, &this->_xfixesEventBase, &this->_xfixesErrorBase)) {
        XFixesSelectCursorInput(this->_x11Display,  this->_rootWindow->getX11Window(), XFixesDisplayCursorNotifyMask);
    } else {
        spdlog::error("No xfixes extension");
        exit(1);
    }
}

WebXEventListener::~WebXEventListener() {
}

void WebXEventListener::flushQueuedEvents() {
    XEvent x11Event;

    XFlush(this->_x11Display);
    int qLength = QLength(this->_x11Display);

    for (int i = 0; i < qLength; i++) {
        XNextEvent(this->_x11Display, &x11Event);
        WebXEvent event(x11Event, this->_damageEventBase);
        this->handleEvent(event);
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
