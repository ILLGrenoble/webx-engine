#ifndef WEBX_CONTROLLER_H
#define WEBX_CONTROLLER_H

#include <vector>
#include <set>
#include <chrono>
#include <thread>
#include <mutex>
#include <string>
#include "WebXStats.h"
#include <gateway/WebXGateway.h>
#include <display/WebXWindowProperties.h>
#include <display/WebXWindowDamageProperties.h>
#include <display/WebXDisplayEventType.h>
#include <utils/WebXQuality.h>

class WebXManager;
class WebXDisplay;
class WebXInstruction;
class WebXMessage;

class WebXController {
public:
    WebXController(WebXGateway & gateway, const std::string & keyboardLayout = "");
    virtual ~WebXController();

    void run();
    void stop();

private:
    enum WebXControllerState {
        Stopped = 0,
        Running
    };

    void onDisplayEvent(WebXDisplayEventType eventType) {
        if (eventType == WebXDisplayEventType::CursorEvent) {
            this->_cursorDirty = true;
        } else if (eventType == WebXDisplayEventType::WindowLayoutEvent) {
            this->_displayDirty = true;
        }
    }

    void setRequestedQuality(uint32_t qualityIndex) {
        this->_requestedQuality = WebXQuality::QualityForIndex(qualityIndex);
    }
    
    void handleClientInstructions(WebXDisplay * display);
    void notifyDisplayChanged(WebXDisplay * display);
    float notifyImagesChanged(WebXDisplay * display);
    void notifyMouseChanged(WebXDisplay * display);

    void sendMessage(std::shared_ptr<WebXMessage> message, uint32_t commandId = 0);

private:
    const static unsigned int THREAD_RATE = 60;
    const static unsigned int DEFAULT_IMAGE_REFRESH_RATE = 30;
    const static unsigned int MOUSE_MIN_REFRESH_DELAY_US = 15000;
    const static unsigned int MOUSE_MAX_REFRESH_DELAY_US = 500000;

    WebXGateway & _gateway;
    WebXManager * _manager;
    WebXStats _stats;

    std::vector<std::shared_ptr<WebXInstruction>> _instructions;

    bool _displayDirty;
    bool _cursorDirty;

    WebXQuality _requestedQuality;

    long _threadSleepUs;
    std::mutex _instructionsMutex;
    WebXControllerState _state;

};


#endif /* WEBX_CONTROLLER_H */