#ifndef WEBX_WINDOWS_MESSAGE_H
#define WEBX_WINDOWS_MESSAGE_H

#include <vector>
#include "WebXMessage.h"
#include <display/WebXWindowProperties.h>

/**
 * @class WebXWindowsMessage
 * @brief Represents a message containing information about the visible windows.
 * 
 * This class is derived from WebXMessage and is used to encapsulate
 * details about the visible windows (eg size and position).
 */
class WebXWindowsMessage : public WebXMessage {
public:
    /**
     * @brief Constructs a WebXWindowsMessage with a command ID.
     * 
     * @param clientIndexMask The client index mask.
     * @param commandId The command ID associated with the message.
     * @param windows A vector of window properties.
     */
    WebXWindowsMessage(uint64_t clientIndexMask, uint32_t commandId, const std::vector<WebXWindowProperties> & windows) :
        WebXMessage(Type::Windows, clientIndexMask, commandId),
        windows(windows) {}

    /**
     * @brief Constructs a WebXWindowsMessage without a command ID.
     * 
     * @param clientIndexMask The client index mask.
     * @param windows A vector of window properties.
     */
    WebXWindowsMessage(uint64_t clientIndexMask, const std::vector<WebXWindowProperties> & windows) :
        WebXMessage(Type::Windows, clientIndexMask),
        windows(windows) {}

    /**
     * @brief Destructor for WebXWindowsMessage.
     */
    virtual ~WebXWindowsMessage() {}

    const std::vector<WebXWindowProperties> windows;
};

#endif /* WEBX_WINDOWS_MESSAGE_H*/