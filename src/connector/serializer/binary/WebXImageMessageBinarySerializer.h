#ifndef WEBX_IMAGE_MESSAGE_BINARY_SERIALIZER_H
#define WEBX_IMAGE_MESSAGE_BINARY_SERIALIZER_H

#include <zmq.hpp>
#include "connector/message/WebXImageMessage.h"
#include "connector/serializer/binary/WebXBinaryMessageSerializer.h"

class WebXImageMessageBinarySerializer : public WebXBinaryMessageSerializer<WebXImageMessage> {
public:
    // Structure:
    // Header: 16 bytes
    //   type: 4 bytes
    //   id: 4 bytes
    //   length: 4 bytes
    //   padding: 4 bytes
    // Content:
    //   commandId: 4 bytes
    //   windowId: 4 bytes
    //   depth: 4 bytes
    //   imageType: 4 bytes (chars)
    //   imageDataLength: 4 bytes
    //   imageData: n bytes
    zmq::message_t * serialize(const WebXImageMessage * message);
};


#endif //WEBX_IMAGE_MESSAGE_BINARY_SERIALIZER_H
