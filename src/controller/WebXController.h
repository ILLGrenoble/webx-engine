#ifndef WEBX_CONTROLLER_H
#define WEBX_CONTROLLER_H

#include <vector>
#include <set>
#include <chrono>
#include <thread>
#include <mutex>
#include <string>
#include "WebXStats.h"
#include <display/WebXManager.h>
#include <gateway/WebXGateway.h>
#include "client/WebXClientRegistry.h"
#include <display/WebXWindowProperties.h>
#include <display/WebXDisplayEventType.h>
#include <models/WebXSettings.h>

class WebXDisplay;
class WebXInstruction;
class WebXMessage;

/**
 * @class WebXController
 * @brief Manages the core logic and state of the WebX engine, including client communication, display updates, and event handling.
 */
class WebXController {
public:
    /**
     * @brief Constructs a WebXController instance.
     * @param gateway Reference to the WebXGateway for communication.
     * @param settings Reference to the WebXSettings for configuration.
     * @param keyboardLayout Optional keyboard layout string.
     */
    WebXController(WebXGateway & gateway, const WebXSettings & settings, const std::string & keyboardLayout = "");

    /**
     * @brief Destructor for WebXController.
     */
    virtual ~WebXController();

    /**
     * @brief Starts the main execution loop of the controller.
     */
    void run();

    /**
     * @brief Stops the main execution loop of the controller.
     */
    void stop();

private:
    /**
     * @enum WebXControllerState
     * @brief Represents the state of the WebXController.
     */
    enum WebXControllerState {
        Stopped = 0,
        Running
    };

    /**
     * @brief Handles display events and updates internal state flags.
     * @param eventType The type of display event.
     */
    void onDisplayEvent(WebXDisplayEventType eventType) {
        if (eventType == WebXDisplayEventType::CursorEvent) {
            this->_cursorDirty = true;
        } else if (eventType == WebXDisplayEventType::WindowLayoutEvent) {
            this->_displayDirty = true;
        }
    }
    
    /**
     * @brief Processes client instructions for a given display.
     * @param display Pointer to the WebXDisplay instance.
     */
    void handleClientInstructions(WebXDisplay * display);

    /**
     * @brief Notifies that the display has changed.
     * @param display Pointer to the WebXDisplay instance.
     */
    void notifyDisplayChanged(WebXDisplay * display);

    /**
     * @brief Handles periodic client ping messages.
     */
    void handleClientPings();

    /**
     * @brief Updates client windows and returns the total data transferred to clients in kilobytes.
     * @param display Pointer to the WebXDisplay instance.
     * @return Total data transferred to clients in kilobytes.
     */
    float updateClientWindows(WebXDisplay * display);

    /**
     * @brief Notifies that the mouse state has changed.
     * @param display Pointer to the WebXDisplay instance.
     */
    void notifyMouseChanged(WebXDisplay * display);

    /**
     * @brief Sends a message to the gateway to be published to clients.
     * @param message Shared pointer to the WebXMessage to be sent.
     */
    void sendMessage(std::shared_ptr<WebXMessage> message) {
        this->_gateway.publishMessage(message);
    }

private:
    const static unsigned int THREAD_RATE = 60;
    const static unsigned int DEFAULT_IMAGE_REFRESH_RATE = 30;
    const static unsigned int MOUSE_MIN_REFRESH_DELAY_US = 15000;
    const static unsigned int MOUSE_MAX_REFRESH_DELAY_US = 500000;
    const static uint64_t GLOBAL_CLIENT_INDEX_MASK; // Sets all bits

    WebXGateway & _gateway;
    const WebXSettings & _settings;
    WebXManager _manager;
    WebXClientRegistry _clientRegistry;
    WebXStats _stats;

    std::vector<std::shared_ptr<WebXInstruction>> _instructions;


    bool _displayDirty;
    bool _cursorDirty;

    long _threadSleepUs;
    std::mutex _instructionsMutex;
    WebXControllerState _state;

};

#endif /* WEBX_CONTROLLER_H */