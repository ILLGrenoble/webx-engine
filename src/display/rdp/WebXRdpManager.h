#ifndef WEBX_RDP_MANAGER_H
#define WEBX_RDP_MANAGER_H

#include <display/WebXManager.h>
#include <models/WebXSettings.h>
#include "WebXRdpClient.h"

class WebXRdpDisplay;

class WebXRdpManager : public WebXManager {
public:
    /**
     * @brief Constructs a WebXManager instance.
     */
    WebXRdpManager(const WebXSettings & settings);

    /**
     * @brief Destructor.
     */
    virtual ~WebXRdpManager();

    /**
     * @brief Retrieves the WebXDisplay instance.
     * @return Pointer to the WebXDisplay instance.
     */
    virtual WebXDisplay * getDisplay() const;

    /**
     * @brief Processes all pending X11 events.
     */
    virtual void handlePendingEvents();

    /**
     * @brief Sets the handler for display-related events.
     * @param handler Function to handle display events.
     */
    virtual void setDisplayEventHandler(std::function<void(WebXDisplayEventType eventType)> handler) {
        this->_onDisplayEvent = handler;
    }

    /**
     * @brief Sets the handler for damage-related events.
     * @param handler Function to handle damage events.
     */
    virtual void setDamageEventHandler(std::function<void(const WebXWindowDamage & damage)> handler) {
        this->_onDamageEvent = handler;
    }

    /**
     * @brief Sets the handler for clipboard-related events.
     * @param handler Function to handle clipboard events.
     */
    virtual void setClipboardEventHandler(std::function<void(const std::string & clipboardContent)> handler) {
        this->_onClipboardEvent = handler;
    }

    /**
     * @brief Sets the handler for screen resize events.
     * @param handler Function to handle screen resize events.
     */
    virtual void setScreenResizeEventHandler(std::function<void(int width, int height)> handler) {
        this->_onScreenResizeEvent = handler;
    }

    /**
     * @brief Sets the content of the clipboard.
     * @param clipboardContent The content to set in the clipboard.
     */
    virtual void setClipboardContent(const std::string & clipboardContent);

private:
    void init();

    void handleInvalidRectangles(const std::vector<WebXRectangle> & invalidRectangles);

    /**
     * @brief Sends a display-related event to the registered handler.
     * @param eventType The type of the display event.
     */
    void sendDisplayEvent(WebXDisplayEventType eventType) {
        this->_onDisplayEvent(eventType);
    }

    /**
     * @brief Sends a damage-related event to the registered handler.
     * @param damage The damage event data.
     */
    void sendDamageEvent(const WebXWindowDamage & damage) {
        this->_onDamageEvent(damage);
    }

    /**
     * @brief Sends a clipboard event to the registered handler.
     * @param clipboardContent The content of the clipboard.
     */
    void sendClipboardEvent(const std::string & clipboardContent) {
        this->_onClipboardEvent(clipboardContent);
    }

    /**
     * @brief Sends a screen resize event to the registered handler.
     * @param width the screen width
     * @param height the screen height
     */
    void sendScreenResizeEvent(int width, int height) {
        this->_onScreenResizeEvent(width, height);
    }


private:
    WebXRdpClient * _rdpClient;
    WebXRdpDisplay * _display;

    std::function<void(WebXDisplayEventType eventType)> _onDisplayEvent;
    std::function<void(const WebXWindowDamage & damage)> _onDamageEvent;
    std::function<void(const std::string & clipboardContent)> _onClipboardEvent;
    std::function<void(int width, int height)> _onScreenResizeEvent;    
};


#endif /* WEBX_RDP_MANAGER_H */