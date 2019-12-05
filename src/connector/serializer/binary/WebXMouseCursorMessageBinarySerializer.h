#ifndef WEBX_MOUSE_CURSOR_MESSAGE_BINARY_SERIALIZER_H
#define WEBX_MOUSE_CURSOR_MESSAGE_BINARY_SERIALIZER_H

#include <zmq.hpp>
#include "connector/message/WebXMouseCursorMessage.h"
#include "connector/serializer/binary/WebXBinaryMessageSerializer.h"

class WebXMouseCursorMessageBinarySerializer : public WebXBinaryMessageSerializer<WebXMouseCursorMessage> {
public:

    // Structure:
    // Header: 16 bytes
    //   type: 4 bytes
    //   id: 4 bytes
    //   length: 4 bytes
    //   padding: 4 bytes
    // Content:
    //   commandId: 4 bytes
    //   imageDataLength: 4 bytes
    //   imageData: n bytes
    zmq::message_t * serialize(const WebXMouseCursorMessage * message);
};


#endif //WEBX_MOUSE_CURSOR_MESSAGE_BINARY_SERIALIZER_H
