#ifndef WEBX_QUALITY_MESSAGE_H
#define WEBX_QUALITY_MESSAGE_H

#include "WebXMessage.h"
#include <models/WebXQuality.h>

/**
 * @class WebXQualityMessage
 * @brief Represents a message containing quality settings.
 * 
 * This class is used to encapsulate quality-related data.
 */
class WebXQualityMessage : public WebXMessage {
public:
    /**
     * @brief Constructs a WebXQualityMessage.
     * 
     * @param clientIndexMask The client index mask.
     * @param quality The quality settings.
     */
    WebXQualityMessage(uint64_t clientIndexMask, const WebXQuality & quality) :
        WebXMessage(Type::Quality, clientIndexMask),
        quality(quality) {}

    /**
     * @brief Destructor for WebXQualityMessage.
     */
    virtual ~WebXQualityMessage() {}

    const WebXQuality & quality;
};

#endif /* WEBX_QUALITY_MESSAGE_H */