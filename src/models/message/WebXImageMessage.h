#ifndef WEBX_IMAGE_MESSAGE_H
#define WEBX_IMAGE_MESSAGE_H

#include <vector>
#include <memory>
#include "WebXMessage.h"

class WebXImage;

/**
 * @class WebXImageMessage
 * @brief Represents a message containing image data.
 * 
 * This class is used to encapsulate image data associated with a specific window (including color map and image map).
 */
class WebXImageMessage : public WebXMessage {
public:
    /**
     * @brief Constructs a WebXImageMessage without a command ID.
     * 
     * @param clientIndexMask The client index mask.
     * @param windowId The ID of the window associated with the image.
     * @param image A shared pointer to the image data.
     */
    WebXImageMessage(uint64_t clientIndexMask, uint32_t windowId, std::shared_ptr<WebXImage> image) :
        WebXMessage(Type::Image, clientIndexMask),
        windowId(windowId),
        image(image) {}

    /**
     * @brief Constructs a WebXImageMessage with a command ID.
     * 
     * @param clientIndexMask The client index mask.
     * @param commandId The command ID associated with the message.
     * @param windowId The ID of the window associated with the image.
     * @param image A shared pointer to the image data.
     */
    WebXImageMessage(uint64_t clientIndexMask, uint32_t commandId, uint32_t windowId, std::shared_ptr<WebXImage> image) :
        WebXMessage(Type::Image, clientIndexMask, commandId),
        windowId(windowId),
        image(image) {}

    /**
     * @brief Destructor for WebXImageMessage.
     */
    virtual ~WebXImageMessage() {}

    const uint32_t windowId;
    const std::shared_ptr<WebXImage> image;
};

#endif /* WEBX_IMAGE_MESSAGE_H*/