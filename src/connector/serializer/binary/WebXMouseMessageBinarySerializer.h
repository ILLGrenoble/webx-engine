#ifndef WEBX_MOUSE_MESSAGE_BINARY_SERIALIZER_H
#define WEBX_MOUSE_MESSAGE_BINARY_SERIALIZER_H

#include "WebXBinaryMessageSerializer.h"
#include <connector/message/WebXMouseCursorMessage.h>

// @TODO Define a mouse structure
class WebXMouseMessageBinarySerializer : public WebXBinaryMessageSerializer<WebXMouseCursorMessage> {
public:
    WebXMouseMessageBinarySerializer() {}
    virtual ~WebXMouseMessageBinarySerializer() {}

    // @TODO Define structure here
    virtual zmq::message_t * serialize(const WebXMouseCursorMessage * message);
};


#endif //WEBX_MOUSE_MESSAGE_BINARY_SERIALIZER_H
