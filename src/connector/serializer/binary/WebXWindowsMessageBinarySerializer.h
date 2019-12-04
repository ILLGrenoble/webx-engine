#ifndef WEBX_WINDOWS_MESSAGE_BINARY_SERIALIZER_H
#define WEBX_WINDOWS_MESSAGE_BINARY_SERIALIZER_H

#include <zmq.hpp>
#include "connector/message/WebXWindowsMessage.h"
#include "connector/serializer/binary/WebXBinaryMessageSerializer.h"

class WebXWindowsMessageBinarySerializer : public WebXBinaryMessageSerializer<WebXWindowsMessage> {
public:
    // Structure:
    // Header: 16 bytes
    //   type: 4 bytes
    //   id: 4 bytes
    //   length: 4 bytes
    //   padding: 4 bytes
    // Content:
    //   commandId: 4 bytes
    //   # windows: 4 bytes
    //   Window: 24 bytes
    //      id: 4 bytes
    //      x: 4 bytes
    //      y: 4 bytes
    //      width: 4 bytes
    //      height: 4 bytes
    zmq::message_t * serialize(const WebXWindowsMessage * message);
};


#endif //WEBX_WINDOWS_MESSAGE_BINARY_SERIALIZER_H
