#ifndef WEBX_CONTROLLER_H
#define WEBX_CONTROLLER_H

#include <vector>
#include <set>
#include <chrono>
#include <thread>
#include <mutex>
#include <display/WebXWindowProperties.h>
#include <display/WebXWindowDamageProperties.h>

class WebXManager;
class WebXDisplay;
class WebXConnection;
class WebXInstruction;
class WebXMessage;

class WebXController {
private:
    struct WebXQuality {
        int imageFPS;
        float imageQuality;
    };

    struct WebXFrameData {
        double fps;
        double duration;
    };


private:
    WebXController();
    virtual ~WebXController();

public:
    static WebXController * instance();
    static void shutdown();

    enum WebXControllerState {
        Stopped = 0,
        Running
    };

    WebXManager * getManager() const {
        return this->_manager;
    }

    void onDisplayChanged() {
        this->_displayDirty = true;
    }

    void onMouseChanged() {
        this->_mouseDirty = true;
    }

    void init();
    void run();
    void stop();

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
    void setQualityIndex(uint32_t qualityIndex);

    void handleClientInstructions(WebXDisplay * display);
    void notifyDisplayChanged(WebXDisplay * display);
    void notifyImagesChanged(WebXDisplay * display);
    void notifyMouseChanged(WebXDisplay * display);

    void sendMessage(std::shared_ptr<WebXMessage> message, uint32_t commandId = 0);
    void updateFrameData(double fps, double duration);

private:
    static WebXController * _instance;

    const static unsigned int THREAD_RATE = 60;
    const static unsigned int DEFAULT_IMAGE_REFRESH_RATE = 30;
    const static unsigned int MOUSE_MIN_REFRESH_RATE = 60;
    const static unsigned int MOUSE_MAX_REFRESH_RATE = 10;
    const static size_t FRAME_DATA_STORE_SIZE = 30;

    static std::vector<WebXQuality> QUALITY_SETTINGS;

    WebXManager * _manager;

    std::vector<WebXWindowProperties> _windows;
    std::vector<std::shared_ptr<WebXInstruction>> _instructions;

    bool _displayDirty;
    bool _mouseDirty;

    long _imageRefreshUs;
    uint32_t _qualityIndex;

    long _threadSleepUs;
    std::mutex _stateMutex;
    std::mutex _connectionMutex;
    std::mutex _windowsMutex;
    std::mutex _instructionsMutex;
    WebXControllerState _state;

    WebXConnection * _connection;
    std::vector<WebXFrameData> _frameDataStore;
    int _frameDataStoreIndex;

};


#endif /* WEBX_CONTROLLER_H */