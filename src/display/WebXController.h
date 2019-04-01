#ifndef WEBX_CONTROLLER_H
#define WEBX_CONTROLLER_H

#include <vector>
#include <set>
#include <chrono>
#include <tinythread/tinythread.h>

class WebXWindow;
class WebXDisplay;
class WebXConnection;

class WebXController {

public:
    enum WebXControllerState {
        Stopped = 0,
        Paused,
        Running
    };
    WebXController(WebXDisplay * display);
    virtual ~WebXController();

    void onDisplayChanged() {
        this->_displayDirty = true;
    }

    void run();
    void stop();
    void pause();
    void resume();

    void addConnection(WebXConnection * connection) {
        tthread::lock_guard<tthread::mutex> lock(this->_connectionsMutex);
        this->_connections.insert(connection);
    } 

    void removeConnection(WebXConnection * connection) {
        tthread::lock_guard<tthread::mutex> lock(this->_connectionsMutex);
        this->_connections.erase(connection);
    }

    const std::vector<WebXWindow *> & getWindows() {
        tthread::lock_guard<tthread::mutex> lock(this->_windowsMutex);
        return this->_windows;
    }

private:
    static void threadMain(void * arg);
    void mainLoop();
    void updateDisplay();

private:
    static unsigned int THREAD_RATE;
    static unsigned int DISPLAY_REFRESH_RATE;

    WebXDisplay * _display;
    std::vector<WebXWindow *> _windows;

    bool _displayDirty;
    long _displayRefreshUs;
    std::chrono::high_resolution_clock::time_point _lastDisplayRefreshTime;

    tthread::thread * _thread;
    long _threadSleepUs;
    tthread::mutex _stateMutex;
    tthread::mutex _connectionsMutex;
    tthread::mutex _windowsMutex;
    WebXControllerState _state;

    std::set<WebXConnection *> _connections;
};


#endif /* WEBX_CONTROLLER_H */