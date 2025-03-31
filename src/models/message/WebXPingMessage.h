#ifndef WEBX_PING_MESSAGE_H
#define WEBX_PING_MESSAGE_H

#include "WebXMessage.h"

/**
 * @class WebXPingMessage
 * @brief Represents a ping message.
 * 
 * This class is used to encapsulate a simple ping message for
 * communication purposes, typically to check connectivity and measure Round-Trip Time latency.
 */
class WebXPingMessage : public WebXMessage {
public:
    /**
     * @brief Constructs a WebXPingMessage.
     * 
     * @param clientIndexMask The client index mask.
     */
    WebXPingMessage(uint64_t clientIndexMask) :
        WebXMessage(Type::Ping, clientIndexMask) {}

    /**
     * @brief Destructor for WebXPingMessage.
     */
    virtual ~WebXPingMessage() {}
};

#endif /* WEBX_PING_MESSAGE_H */