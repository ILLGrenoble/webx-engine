#ifndef WEBX_CONTROLLER_H
#define WEBX_CONTROLLER_H

#include <vector>
#include <set>
#include <chrono>
#include <tinythread/tinythread.h>
#include <connector/instruction/WebXInstruction.h>
#include "WebXWindowProperties.h"
#include "WebXWindowDamageProperties.h"

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

    void onMouseChanged() {
        this->_mouseDirty = true;
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

    void onClientInstruction(std::shared_ptr<WebXInstruction> instruction) {
        tthread::lock_guard<tthread::mutex> lock(this->_instructionsMutex);
        this->_instructions.push_back(instruction);
    }

private:
    static void threadMain(void * arg);
    void mainLoop();
    void handleClientInstructions();
    void notifyDisplayChanged();
    void notifyImagesChanged();
    void notifyMouseChanged();
    void updateFps(double fps);

private:
    static unsigned int THREAD_RATE;
    static unsigned int IMAGE_REFRESH_RATE;
    static unsigned int MOUSE_MIN_REFRESH_RATE;
    static unsigned int MOUSE_MAX_REFRESH_RATE;

    WebXDisplay * _display;
    std::vector<WebXWindowProperties> _windows;
    std::vector<std::shared_ptr<WebXInstruction>> _instructions;

    bool _displayDirty;
    bool _mouseDirty;

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