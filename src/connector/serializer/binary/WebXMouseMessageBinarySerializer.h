#ifndef WEBX_MOUSE_MESSAGE_BINARY_SERIALIZER_H
#define WEBX_MOUSE_MESSAGE_BINARY_SERIALIZER_H

#include "WebXBinaryMessageSerializer.h"
#include <connector/message/WebXMouseMessage.h>

// @TODO Define a mouse structure
class WebXMouseMessageBinarySerializer : public WebXBinaryMessageSerializer<WebXMouseMessage> {
public:
    WebXMouseMessageBinarySerializer() {}
    virtual ~WebXMouseMessageBinarySerializer() {}

    // Structure:
    // Header: 16 bytes
    //   type: 4 bytes
    //   id: 4 bytes
    //   length: 4 bytes
    //   padding: 4 bytes
    // Content:
    //   commandId: 4 bytes
    //   x: 4 bytes
    //   y: 4 bytes
    //   cursorId: 4 bytes
    virtual zmq::message_t * serialize(const WebXMouseMessage * message);
};


#endif //WEBX_MOUSE_MESSAGE_BINARY_SERIALIZER_H
