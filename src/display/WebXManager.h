#ifndef WEBX_MANAGER_H
#define WEBX_MANAGER_H

#include <X11/Xlib.h>
#include <string>
#include <vector>
#include <functional>
#include "WebXDisplayEventType.h"
#include "events/WebXEvent.h"
#include <models/WebXSettings.h>
#include <models/WebXWindowDamage.h>

class WebXWindow;
class WebXDisplay;
class WebXEventListener;
class WebXClipboard;

/**
 * @class WebXManager
 * @brief Manages the interaction with the X11 display and handles events.
 * 
 * This class is responsible for initializing the X11 display, handling window
 * and damage events, and providing an interface for managing the display and
 * its associated windows.
 */
class WebXManager {

public:
    /**
     * @brief Handles X11 I/O errors.
     * @param disp Pointer to the X11 display.
     * @return Error code.
     */
    static int IO_ERROR_HANDLER(Display *disp);

    /**
     * @brief Constructs a WebXManager instance.
     * @param settings Reference to the WebXSettings instance.
     * @param keyboardLayout Optional keyboard layout string.
     */
    WebXManager(const WebXSettings & settings, const std::string & keyboardLayout = "");

    /**
     * @brief Destructor.
     */
    virtual ~WebXManager();

    /**
     * @brief Retrieves the WebXDisplay instance.
     * @return Pointer to the WebXDisplay instance.
     */
    WebXDisplay * getDisplay() const {
        return this->_display;
    }

    /**
     * @brief Processes all pending X11 events.
     */
    void handlePendingEvents();

    /**
     * @brief Sets the handler for display-related events.
     * @param handler Function to handle display events.
     */
    void setDisplayEventHandler(std::function<void(WebXDisplayEventType eventType)> handler) {
        this->_onDisplayEvent = handler;
    }

    /**
     * @brief Sets the handler for damage-related events.
     * @param handler Function to handle damage events.
     */
    void setDamageEventHandler(std::function<void(const WebXWindowDamage & damage)> handler) {
        this->_onDamageEvent = handler;
    }

    /**
     * @brief Sets the handler for clipboard-related events.
     * @param handler Function to handle clipboard events.
     */
    void setClipboardEventHandler(std::function<void(const std::string & clipboardContent)> handler) {
        this->_onClipboardEvent = handler;
    }

    /**
     * @brief Sets the content of the clipboard.
     * @param clipboardContent The content to set in the clipboard.
     */
    void setClipboardContent(const std::string & clipboardContent);

private:
    /**
     * @brief Initializes the manager with the specified keyboard layout.
     * @param keyboardLayout Optional keyboard layout string.
     */
    void init(const std::string & keyboardLayout = "");

    /**
     * @brief Handles the creation of a window.
     * @param event The event data.
     */
    void handleWindowCreateEvent(const WebXEvent & event);

    /**
     * @brief Handles the destruction of a window.
     * @param event The event data.
     */
    void handleWindowDestroyEvent(const WebXEvent & event);

    /**
     * @brief Handles the mapping of a window.
     * @param event The event data.
     */
    void handleWindowMapEvent(const WebXEvent & event);

    /**
     * @brief Handles the unmapping of a window.
     * @param event The event data.
     */
    void handleWindowUnmapEvent(const WebXEvent & event);

    /**
     * @brief Handles the reparenting of a window.
     * @param event The event data.
     */
    void handleWindowReparentEvent(const WebXEvent & event);

    /**
     * @brief Handles the configuration of a window.
     * @param event The event data.
     */
    void handleWindowConfigureEvent(const WebXEvent & event);

    /**
     * @brief Handles the gravity changes of a window.
     * @param event The event data.
     */
    void handleWindowGravityEvent(const WebXEvent & event);

    /**
     * @brief Handles the circulation of a window.
     * @param event The event data.
     */
    void handleWindowCirculateEvent(const WebXEvent & event);

    /**
     * @brief Handles damage events for a window.
     * @param event The event data.
     */
    void handleWindowDamageEvent(const WebXEvent & event);

    /**
     * @brief Handles mouse cursor events.
     * @param event The event data.
     */
    void handleMouseCursorEvent(const WebXEvent & event);

    /**
     * @brief Updates the display state.
     */
    void updateDisplay();

    /**
     * @brief Sends a display-related event to the registered handler.
     * @param eventType The type of the display event.
     */
    void sendDisplayEvent(WebXDisplayEventType eventType) {
        if (this->_onDisplayEvent) {
            this->_onDisplayEvent(eventType);
        }
    }

    /**
     * @brief Sends a damage-related event to the registered handler.
     * @param damage The damage event data.
     */
    void sendDamageEvent(const WebXWindowDamage & damage) {
        if (this->_onDamageEvent) {
            this->_onDamageEvent(damage);
        }
    }

    /**
     * @brief Sends a clipboard event to the registered handler.
     * @param clipboardContent The content of the clipboard.
     */
    void sendClipboardEvent(const std::string & clipboardContent) {
        if (this->_onClipboardEvent) {
            this->_onClipboardEvent(clipboardContent);
        }
    }

private:
    const WebXSettings & _settings;

    Display * _x11Display;
    WebXDisplay * _display;
    WebXEventListener * _eventListener;
    WebXClipboard * _clipboard;
    bool _displayRequiresUpdate;

    std::function<void(WebXDisplayEventType eventType)> _onDisplayEvent;
    std::function<void(const WebXWindowDamage & damage)> _onDamageEvent;
    std::function<void(const std::string & clipboardContent)> _onClipboardEvent;
};


#endif /* WEBX_MANAGER_H */