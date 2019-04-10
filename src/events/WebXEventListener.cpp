#include "WebXEventListener.h"
#include "WebXEvent.h"
#include <display/WebXWindow.h>
#include <X11/Xatom.h>
#include <X11/extensions/Xdamage.h>
#include <X11/extensions/Xrender.h>
#include <X11/extensions/shape.h>
#include <X11/Xutil.h>


WebXEventListener::WebXEventListener(Display * display) :
    _x11Display(display),
    _thread(NULL),
    _stopped(false),
    _paused(false),
    _damageEventBase(0),
    _damageErrorBase(0),
    _damageAvailable(false) {

    if (XDamageQueryExtension(this->_x11Display, &this->_damageEventBase, &this->_damageErrorBase)) {
        this->_damageAvailable = true;
    
    } else {
        fprintf(stderr, "No damage extension\n");
        exit(1);
    }
}

WebXEventListener::~WebXEventListener() {
    this->stop();
}

void WebXEventListener::run(WebXWindow * rootWindow) {
    tthread::lock_guard<tthread::mutex> lock(this->_mutex);
    this->_rootWindow = rootWindow;

    // Init dummy event
    this->_dummyEvent.type = ClientMessage;
    this->_dummyEvent.window = this->_rootWindow->getX11Window();
    this->_dummyEvent.format = 32;

    if (!this->_stopped && this->_thread == NULL) {
        this->_thread = new tthread::thread(WebXEventListener::threadMain, (void *)this);
    }
}

void WebXEventListener::stop() {
    tthread::lock_guard<tthread::mutex> lock(this->_mutex);
    if (_thread != NULL && !this->_stopped) {
        this->_stopped = true;

        // Send dummy event to unblock XNextEvent
        XSendEvent(this->_x11Display, this->_rootWindow->getX11Window(), 0, SubstructureNotifyMask, (XEvent*)&this->_dummyEvent);
        XFlush(this->_x11Display);

        // Join thread and cleanup
        printf("Stopping event listener...\n");
        this->_thread->join();
        printf("... stopped event listener\n");
        delete this->_thread;
        this->_thread = NULL;
        this->_rootWindow = NULL;
    }
}

void WebXEventListener::pause() {
    tthread::lock_guard<tthread::mutex> lock(this->_mutex);
    if (_thread != NULL && !this->_paused) {
        this->_paused = true;

        // Send dummy event to unblock XNextEvent
        XSendEvent(this->_x11Display, this->_rootWindow->getX11Window(), 0, SubstructureNotifyMask, (XEvent*)&this->_dummyEvent);
        XFlush(this->_x11Display);
    }
}

void WebXEventListener::resume() {
    tthread::lock_guard<tthread::mutex> lock(this->_mutex);
    if (_thread != NULL && this->_paused) {
        this->_pauseQueue.put(new bool(true));
        this->_paused = false;
    }
}

void WebXEventListener::threadMain(void * arg) {
    WebXEventListener * self = (WebXEventListener *)arg;
    self->mainLoop();
}

void WebXEventListener::mainLoop() {
    XEvent x11Event;

    XSelectInput(this->_x11Display, this->_rootWindow->getX11Window(), SubstructureNotifyMask);

    while (!this->_stopped) {
        do {
            XNextEvent(this->_x11Display, &x11Event);

            if (this->_paused) {
                // Wait until resumed
                bool * value = this->_pauseQueue.get();
                delete value;

            } else if (!this->_stopped) {
                WebXEvent event(x11Event, this->_damageEventBase);
                this->handleEvent(event);
            }
        } while (!this->_stopped && QLength(this->_x11Display));
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
