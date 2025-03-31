#ifndef WEBX_MESSAGE_H
#define WEBX_MESSAGE_H

#include <string>
#include <cstdint>

/**
 * @class WebXMessage
 * @brief Base class for all WebX messages.
 * 
 * This class serves as the base for various message types used in
 * communication between the WebX Engine and clients. It provides common properties
 * such as message type, client index mask, and command ID.
 */
class WebXMessage {
public:
    enum Type {
        Windows = 2,
        Image,
        Screen,
        Subimages,
        Mouse,
        CursorImage,
        Ping,
        Disconnect,
        Quality,
    };

    WebXMessage(Type type, uint64_t clientIndexMask) :
        type(type),
        clientIndexMask(clientIndexMask),
        commandId(-1) {}

    WebXMessage(Type type, uint64_t clientIndexMask, uint32_t commandId) :
        type(type),
        clientIndexMask(clientIndexMask),
        commandId(commandId) {}

    virtual ~WebXMessage() {}

    const Type type;
    const uint64_t clientIndexMask;
    const uint32_t commandId;
};


#endif /* WEBX_MESSAGE_H */