#ifndef WEBX_SUB_IMAGES_MESSAGE_H
#define WEBX_SUB_IMAGES_MESSAGE_H

#include <vector>
#include "WebXMessage.h"
#include <image/WebXSubImage.h>

/**
 * @class WebXSubImagesMessage
 * @brief Represents a message containing sub-image data.
 * 
 * This class is used to encapsulate information about sub-images (color map and alpha map with size and location)
 * associated with a specific window.
 */
class WebXSubImagesMessage : public WebXMessage {
public:
    /**
     * @brief Constructs a WebXSubImagesMessage.
     * 
     * @param clientIndexMask The client index mask.
     * @param windowId The ID of the window associated with the sub-images.
     * @param images A vector of sub-image data.
     */
    WebXSubImagesMessage(uint64_t clientIndexMask, uint32_t windowId, const std::vector<WebXSubImage> & images) :
        WebXMessage(Type::Subimages, clientIndexMask),
        windowId(windowId),
        images(images) {}

    /**
     * @brief Destructor for WebXSubImagesMessage.
     */
    virtual ~WebXSubImagesMessage() {}

    const uint32_t windowId;
    const std::vector<WebXSubImage> images;
};

#endif /* WEBX_SUB_IMAGES_MESSAGE_H*/