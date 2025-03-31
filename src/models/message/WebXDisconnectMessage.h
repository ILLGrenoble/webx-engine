#ifndef WEBX_DISCONNECT_MESSAGE_H
#define WEBX_DISCONNECT_MESSAGE_H

#include "WebXMessage.h"

/**
 * @class WebXDisconnectMessage
 * @brief Represents a disconnect message.
 * 
 * This class is used to encapsulate a message indicating a client
 * disconnection.
 */
class WebXDisconnectMessage : public WebXMessage {
public:
    /**
     * @brief Constructs a WebXDisconnectMessage.
     * 
     * @param clientIndexMask The client index mask.
     */
    WebXDisconnectMessage(uint64_t clientIndexMask) :
        WebXMessage(Type::Disconnect, clientIndexMask) {}

    /**
     * @brief Destructor for WebXDisconnectMessage.
     */
    virtual ~WebXDisconnectMessage() {}
};

#endif /* WEBX_DISCONNECT_MESSAGE_H */