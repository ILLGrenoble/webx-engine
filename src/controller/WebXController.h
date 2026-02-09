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
#include <models/message/WebXClipboardMessage.h>
#include <models/message/WebXScreenResizeMessage.h>

class WebXDisplay;
class WebXInstruction;
class WebXMouse;
class WebXMouseInstruction;
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
     * @param testing Indicates whether the controller is being run during a test
     */
    void run(bool testing = false);

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
     * @struct WebXImageUpdateVerification
     * @brief Represents the verification data for image updates.
     * @param rgbChecksum Checksum for the RGB data.
     * @param alphaChecksum Checksum for the alpha data.
     * @param hasChanged Flag indicating if the image has changed.
     */
    struct WebXImageUpdateVerification {
        uint32_t rgbChecksum;
        uint32_t alphaChecksum;
        bool hasChanged;
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
     * @brief Handles clipboard events (clipboard content changed) and sends them to clients.
     * @param clipboardContent The content of the clipboard.
     */
    void onClipboardEvent(const std::string & clipboardContent) const {
        auto message = std::make_shared<WebXClipboardMessage>(GLOBAL_CLIENT_INDEX_MASK, clipboardContent);
        this->sendMessage(message);
    }
   
    /**
     * @brief Handles screen resize events and notifies clients.
     * @param width the screen width.
     * @param height the screen height.
     */
    void onScreenResizeEvent(int width, int height) const {
        auto message = std::make_shared<WebXScreenResizeMessage>(GLOBAL_CLIENT_INDEX_MASK, WebXSize(width, height));
        this->sendMessage(message);
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
     * Handles client mouse instructions. Updates the display and sends the mouse position to clients.
     * @param display Pointer to the WebXDisplay instance.
     * @param instruction Shared pointer to the WebXInstruction instance.
     * @param client Shared pointer to the WebXClient that sent the instruction.
     */
    void onClientMouseInstruction(WebXDisplay * display, const std::shared_ptr<WebXMouseInstruction> & mouseInstruction, const std::shared_ptr<WebXClient> &);

    /**
     * @brief Notifies that the mouse cursor has changed.
     * @param mouse Pointer to the WebXMouse instance.
     */
    void notifyCursorChanged(WebXMouse * mouse);

    /**
     * @brief Notifies that the mouse position has changed.
     * @param mouse Pointer to the WebXMouse instance.
     */
    void notifyMousePositionChanged(WebXMouse * mouse);

    /**
     * @brief Verifies if an image update is needed and returns the verification data.
     * @param image Shared pointer to the WebXImage to be verified.
     * @param window Unique pointer to the WebXClientWindow to be verified.
     * @return Verification data containing checksums and change status.
     * @note This function checks if the image has changed and updates the alpha channel if necessary.
     * @note It also handles the case where the image is null.
     */
    WebXImageUpdateVerification verifyImageUpdate(std::shared_ptr<WebXImage> & image, const std::unique_ptr<WebXClientWindow> & window);

    /**
     * @brief Sends a message to the gateway to be published to clients.
     * @param message Shared pointer to the WebXMessage to be sent.
     */
    void sendMessage(std::shared_ptr<WebXMessage> message) const {
        this->_gateway.publishMessage(message);
    }

private:
    const static unsigned int THREAD_RATE = 60;
    const static unsigned int DEFAULT_IMAGE_REFRESH_RATE = 30;
    const static unsigned int MOUSE_REFRESH_DELAY_MS = 100;
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