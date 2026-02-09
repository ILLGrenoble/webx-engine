#ifndef WEBX_SCREEN_RESIZE_MESSAGE_H
#define WEBX_SCREEN_RESIZE_MESSAGE_H

#include "WebXMessage.h"
#include <models/WebXSize.h>
#include <models/WebXVersion.h>
#include <string>

/**
 * @class WebXScreenResizeMessage
 * @brief Represents a message containing new screen size.
 */
class WebXScreenResizeMessage : public WebXMessage {
public:
    /**
     * @brief Constructs a WebXScreenResizeMessage.
     * 
     * @param clientIndexMask The client index mask.
     * @param screenSize The size of the screen.
     */
    WebXScreenResizeMessage(uint64_t clientIndexMask, WebXSize screenSize) :
        WebXMessage(Type::ScreenResize, clientIndexMask),
        screenSize(screenSize) {}
    
    /**
     * @brief Destructor for WebXScreenResizeMessage.
     */
    virtual ~WebXScreenResizeMessage() {}

    const WebXSize screenSize;
};

#endif /* WEBX_SCREEN_RESIZE_MESSAGE_H */