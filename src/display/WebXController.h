#ifndef WEBX_CONTROLLER_H
#define WEBX_CONTROLLER_H

#include <vector>
#include <set>
#include <chrono>
#include <tinythread/tinythread.h>
#include "WebXWindowProperties.h"
#include "WebXWindowDamageProperties.h"

class WebXDisplay;
class WebXConnection;
class WebXInstruction;

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

    void onMouseCursorChanged() {
        this->_mouseCursorDirty = true;
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

    const std::vector<WebXWindowProperties> & getWindows() {
        tthread::lock_guard<tthread::mutex> lock(this->_windowsMutex);
        return this->_windows;
    }

    void onClientInstruction(WebXInstruction * instruction) {
        tthread::lock_guard<tthread::mutex> lock(this->_instructionsMutex);
        this->_instructions.push_back(instruction);
    }

private:
    static void threadMain(void * arg);
    void mainLoop();
    void handleClientInstructions();
    void updateDisplay();
    void updateImages();
    void updateFps(double fps);
    void updateMouseCursor();

private:
    static unsigned int THREAD_RATE;
    static unsigned int IMAGE_REFRESH_RATE;

    WebXDisplay * _display;
    std::vector<WebXWindowProperties> _windows;
    std::vector<WebXInstruction *> _instructions;

    bool _displayDirty;
    bool _mouseCursorDirty;

    long _imageRefreshUs;

    tthread::thread * _thread;
    long _threadSleepUs;
    tthread::mutex _stateMutex;
    tthread::mutex _connectionsMutex;
    tthread::mutex _windowsMutex;
    tthread::mutex _instructionsMutex;
    WebXControllerState _state;

    std::set<WebXConnection *> _connections;
    std::vector<double> _fpsStore;
    const static size_t FPS_STORE_SIZE = 30;
    int _fpsStoreIndex;

};


#endif /* WEBX_CONTROLLER_H */