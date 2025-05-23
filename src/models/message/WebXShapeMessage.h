#ifndef WEBX_SHAPE_MESSAGE_H
#define WEBX_SHAPE_MESSAGE_H

#include <vector>
#include <memory>
#include "WebXMessage.h"

class WebXImage;

/**
 * @class WebXShapeMessage
 * @brief Represents a message containing window shape data (stencil map).
 * 
 * This class is used to carry shape data associated with a specific window.
 */
class WebXShapeMessage : public WebXMessage {
public:
    /**
     * @brief Constructs a WebXShapeMessage without a command ID.
     * 
     * @param clientIndexMask The client index mask.
     * @param windowId The ID of the window associated with the shape.
     * @param shape A shared pointer to the shape image data.
     */
    WebXShapeMessage(uint64_t clientIndexMask, uint32_t windowId, std::shared_ptr<WebXImage> shape) :
        WebXMessage(Type::Shape, clientIndexMask),
        windowId(windowId),
        shape(shape) {}

    /**
     * @brief Constructs a WebXShapeMessage with a command ID.
     * 
     * @param clientIndexMask The client index mask.
     * @param commandId The command ID associated with the message.
     * @param windowId The ID of the window associated with the shape.
     * @param shape A shared pointer to the shape image data.
     */
    WebXShapeMessage(uint64_t clientIndexMask, uint32_t commandId, uint32_t windowId, std::shared_ptr<WebXImage> shape) :
        WebXMessage(Type::Shape, clientIndexMask, commandId),
        windowId(windowId),
        shape(shape) {}

    /**
     * @brief Destructor for WebXShapeMessage.
     */
    virtual ~WebXShapeMessage() {}

    const uint32_t windowId;
    const std::shared_ptr<WebXImage> shape;
};

#endif /* WEBX_SHAPE_MESSAGE_H*/