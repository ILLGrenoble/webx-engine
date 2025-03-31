#ifndef WEBX_CURSOR_IMAGE_MESSAGE_H
#define WEBX_CURSOR_IMAGE_MESSAGE_H

#include <utility>
#include <memory>
#include "WebXMessage.h"
#include <image/WebXImage.h>

/**
 * @class WebXCursorImageMessage
 * @brief Represents a message containing cursor image data.
 * 
 * This class is used to encapsulate information about a cursor image,
 * including its position, hotspot, and associated image data.
 */
class WebXCursorImageMessage : public WebXMessage {
public:
    /**
     * @brief Constructs a WebXCursorImageMessage.
     * 
     * @param clientIndexMask The client index mask.
     * @param commandId The command ID associated with the message.
     * @param x The x-coordinate of the cursor.
     * @param y The y-coordinate of the cursor.
     * @param xhot The x-coordinate of the cursor's hotspot.
     * @param yhot The y-coordinate of the cursor's hotspot.
     * @param cursorId The ID of the cursor.
     * @param mouseCursorImage A shared pointer to the cursor image data.
     */
    WebXCursorImageMessage(uint64_t clientIndexMask, uint32_t commandId, int x, int y, int xhot, int yhot, uint32_t cursorId, std::shared_ptr<WebXImage> mouseCursorImage) :
        WebXMessage(Type::CursorImage, clientIndexMask, commandId),
        x(x),
        y(y),
        xhot(xhot),
        yhot(yhot),
        cursorId(cursorId),
        mouseCursorImage(mouseCursorImage) {}

    /**
     * @brief Destructor for WebXCursorImageMessage.
     */
    virtual ~WebXCursorImageMessage() {}

    const int x;
    const int y;
    const int xhot;
    const int yhot;
    const uint32_t cursorId;
    const std::shared_ptr<WebXImage> mouseCursorImage;
};

#endif /* WEBE_CURSOR_IMAGE_MESSAGE_H */