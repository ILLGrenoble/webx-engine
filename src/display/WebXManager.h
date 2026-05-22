#ifndef WEBX_MANAGER_H
#define WEBX_MANAGER_H

#include <display/WebXDisplayEventType.h>
#include <models/WebXSettings.h>
#include <models/WebXWindowDamage.h>

class WebXDisplay;

class WebXManager {

public:
    /**
     * @brief Constructs a WebXManager instance.
     */
    WebXManager() {}

    /**
     * @brief Destructor.
     */
    virtual ~WebXManager() {}

    /**
     * @brief Retrieves the WebXDisplay instance.
     * @return Pointer to the WebXDisplay instance.
     */
    virtual WebXDisplay * getDisplay() const = 0;

    /**
     * @brief Processes all pending X11 events.
     */
    virtual void handlePendingEvents() = 0;

    /**
     * @brief Sets the handler for display-related events.
     * @param handler Function to handle display events.
     */
    virtual void setDisplayEventHandler(std::function<void(WebXDisplayEventType eventType)> handler) = 0;

    /**
     * @brief Sets the handler for damage-related events.
     * @param handler Function to handle damage events.
     */
    virtual void setDamageEventHandler(std::function<void(const WebXWindowDamage & damage)> handler) = 0;

    /**
     * @brief Sets the handler for clipboard-related events.
     * @param handler Function to handle clipboard events.
     */
    virtual void setClipboardEventHandler(std::function<void(const std::string & clipboardContent)> handler) = 0;

    /**
     * @brief Sets the handler for screen resize events.
     * @param handler Function to handle screen resize events.
     */
    virtual void setScreenResizeEventHandler(std::function<void(int width, int height)> handler) = 0;

    /**
     * @brief Sets the content of the clipboard.
     * @param clipboardContent The content to set in the clipboard.
     */
    virtual void setClipboardContent(const std::string & clipboardContent) = 0;

};


#endif /* WEBX_MANAGER_H */