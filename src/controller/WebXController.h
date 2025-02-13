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
class WebXGateway;

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

    void init(WebXGateway * gateway);
    void run();
    void stop();

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
    const static unsigned int MOUSE_MIN_REFRESH_DELAY_US = 15000;
    const static unsigned int MOUSE_MAX_REFRESH_DELAY_US = 500000;
    const static size_t FRAME_DATA_STORE_SIZE = 30;

    static std::vector<WebXQuality> QUALITY_SETTINGS;

    WebXManager * _manager;
    WebXGateway * _gateway;

    std::vector<std::shared_ptr<WebXInstruction>> _instructions;

    bool _displayDirty;
    bool _mouseDirty;

    long _imageRefreshUs;
    uint32_t _qualityIndex;

    long _threadSleepUs;
    std::mutex _instructionsMutex;
    WebXControllerState _state;

    std::vector<WebXFrameData> _frameDataStore;
    int _frameDataStoreIndex;

};


#endif /* WEBX_CONTROLLER_H */