#ifndef WEBX_CONTROLLER_H
#define WEBX_CONTROLLER_H

#include <vector>
#include <set>
#include <chrono>
#include <thread>
#include <mutex>
#include "WebXWindowProperties.h"
#include "WebXWindowDamageProperties.h"

class WebXDisplay;
class WebXConnection;
class WebXInstruction;
class WebXMessage;

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

    void setConnection(WebXConnection * connection) {
        const std::lock_guard<std::mutex> lock(this->_connectionMutex);
        this->_connection = connection;
    } 

    void removeConnection() {
        const std::lock_guard<std::mutex> lock(this->_connectionMutex);
        this->_connection = NULL;
    }

    const std::vector<WebXWindowProperties> & getWindows() {
        const std::lock_guard<std::mutex> lock(this->_windowsMutex);
        return this->_windows;
    }

    void onClientInstruction(std::shared_ptr<WebXInstruction> instruction) {
        const std::lock_guard<std::mutex> lock(this->_instructionsMutex);
        this->_instructions.push_back(instruction);
    }

private:
    void mainLoop();
    void handleClientInstructions();
    void sendMessage(std::shared_ptr<WebXMessage> message, uint32_t commandId = 0);
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

    std::thread * _thread;
    long _threadSleepUs;
    std::mutex _stateMutex;
    std::mutex _connectionMutex;
    std::mutex _windowsMutex;
    std::mutex _instructionsMutex;
    WebXControllerState _state;

    WebXConnection * _connection;
    std::vector<double> _fpsStore;
    const static size_t FPS_STORE_SIZE = 30;
    int _fpsStoreIndex;

};


#endif /* WEBX_CONTROLLER_H */