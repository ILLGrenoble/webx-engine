#ifndef WEBX_SCREEN_MESSAGE_H
#define WEBX_SCREEN_MESSAGE_H

#include "WebXMessage.h"
#include <models/WebXSize.h>
#include <models/WebXVersion.h>
#include <string>

/**
 * @class WebXScreenMessage
 * @brief Represents a message containing screen size and quality information.
 * 
 * This class is used to encapsulate screen size data.
 */
class WebXScreenMessage : public WebXMessage {
public:
    /**
     * @brief Constructs a WebXScreenMessage.
     * 
     * @param clientIndexMask The client index mask.
     * @param commandId The command ID associated with the message.
     * @param screenSize The size of the screen.
     * @param engineVersion The version of the WebX Engine.
     * @param maxQualityIndex The maximum quality index.
     */
    WebXScreenMessage(uint64_t clientIndexMask, uint32_t commandId, WebXSize screenSize, int maxQualityIndex, const WebXVersion & engineVersion) :
        WebXMessage(Type::Screen, clientIndexMask, commandId),
        screenSize(screenSize),
        maxQualityIndex(maxQualityIndex),
        engineVersion(engineVersion) {}
    
    /**
     * @brief Destructor for WebXScreenMessage.
     */
    virtual ~WebXScreenMessage() {}

    const WebXSize screenSize;
    const int maxQualityIndex;
    const WebXVersion engineVersion;
};

#endif /* WEBX_SCREEN_MESSAGE_H */