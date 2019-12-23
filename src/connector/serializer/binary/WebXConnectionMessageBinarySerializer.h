#ifndef WEBX_CONNECTION_MESSAGE_BINARY_SERIALIZER_H
#define WEBX_CONNECTION_MESSAGE_BINARY_SERIALIZER_H

#include "WebXBinaryMessageSerializer.h"
#include <connector/message/WebXConnectionMessage.h>

class WebXConnectionMessageBinarySerializer : public WebXBinaryMessageSerializer<WebXConnectionMessage> {
public:
    WebXConnectionMessageBinarySerializer() {}
    virtual ~WebXConnectionMessageBinarySerializer() {}

    // Structure:
    // Header: 16 bytes
    //   type: 4 bytes
    //   id: 4 bytes
    //   length: 4 bytes
    //   padding: 4 bytes
    // Content:
    //   commandId: 4 bytes
    //   publisherPort: 4 bytes
    //   collectorPort: 4 bytes
    virtual zmq::message_t * serialize(const WebXConnectionMessage * message);
};


#endif //WEBX_CONNECTION_MESSAGE_BINARY_SERIALIZER_H
