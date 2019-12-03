#ifndef WEBX_MOUSE_CURSOR_MESSAGE_BINARY_SERIALIZER_H
#define WEBX_MOUSE_CURSOR_MESSAGE_BINARY_SERIALIZER_H

#include <zmq.hpp>
#include "connector/message/WebXMouseCursorMessage.h"
#include "connector/serializer/binary/WebXBinaryMessageSerializer.h"

class WebXMouseCursorMessageBinarySerializer : public WebXBinaryMessageSerializer<WebXMouseCursorMessage> {
public:

    // @TODO Define structure here
    zmq::message_t * serialize(WebXMouseCursorMessage * message);
};


#endif //WEBX_MOUSE_CURSOR_MESSAGE_BINARY_SERIALIZER_H
