#ifndef WEBX_MOUSE_MESSAGE_H
#define WEBX_MOUSE_MESSAGE_H

#include "WebXMessage.h"

/**
 * @class WebXMouseMessage
 * @brief Represents a message containing mouse event data.
 * 
 * This class is used to encapsulate information about mouse events,
 * such as position and cursor ID.
 */
class WebXMouseMessage : public WebXMessage {
public:
    /**
     * @brief Constructs a WebXMouseMessage.
     * 
     * @param clientIndexMask The client index mask.
     * @param x The x-coordinate of the mouse event.
     * @param y The y-coordinate of the mouse event.
     * @param cursorId The ID of the cursor.
     */
    WebXMouseMessage(uint64_t clientIndexMask, int x, int y, uint32_t cursorId) :
        WebXMessage(Type::Mouse, clientIndexMask),
        x(x),
        y(y),
        cursorId(cursorId) {}

    /**
     * @brief Destructor for WebXMouseMessage.
     */
    virtual ~WebXMouseMessage() {}

    const int x;
    const int y;
    const uint32_t cursorId;
};

#endif /* WEBX_MOUSE_MESSAGE_H */