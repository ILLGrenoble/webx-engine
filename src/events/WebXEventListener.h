#ifndef WEBX_EVENT_LISTENER_H
#define WEBX_EVENT_LISTENER_H

#include <X11/Xlib.h>
#include <map>
#include <functional>
#include "WebXEvent.h"
#include <utils/WebXQueue.h>
#include <tinythread/tinythread.h>

class WebXWindow;

class WebXEventListener {
public:
    WebXEventListener(Display * display, WebXWindow * rootWindow);
    virtual ~WebXEventListener();

    void run();
    void stop();
    void pause();
    void resume();

    void flushQueuedEvents();

    void addEventHandler(WebXEventType eventType, std::function<void(const WebXEvent &)> handler);
    void removeEventHandler(WebXEventType eventType);

    bool getDamageAvailable() const {
        return this->_damageAvailable;
    }

private:
    static void threadMain(void * arg);
    void mainLoop();
    void handleEvent(const WebXEvent & event);

private:
    Display * _x11Display;
    WebXWindow * _rootWindow;
    std::map<WebXEventType, std::function<void(const WebXEvent &)>> _eventHandlers;
    tthread::thread * _thread;
    tthread::mutex _mutex;
    bool _stopped;
    bool _paused;
    WebXQueue<bool> _pauseQueue;
    WebXQueue<bool> _pauseAckQueue;
    XClientMessageEvent _dummyEvent;

    int _damageEventBase;
    int _damageErrorBase;
    bool _damageAvailable;
};


#endif /* WEBX_EVENT_LISTENER_H */