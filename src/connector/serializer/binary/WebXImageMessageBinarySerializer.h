#ifndef WEBX_IMAGE_MESSAGE_BINARY_SERIALIZER_H
#define WEBX_IMAGE_MESSAGE_BINARY_SERIALIZER_H

#include "WebXBinaryMessageSerializer.h"
#include <connector/message/WebXImageMessage.h>

class WebXImageMessageBinarySerializer : public WebXBinaryMessageSerializer<WebXImageMessage> {
public:
    WebXImageMessageBinarySerializer() {}
    virtual ~WebXImageMessageBinarySerializer() {}

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
    virtual zmq::message_t * serialize(const WebXImageMessage * message);
};


#endif //WEBX_IMAGE_MESSAGE_BINARY_SERIALIZER_H
