#ifndef WEBX_VOID_MESSAGE_BINARY_SERIALIZER_H
#define WEBX_VOID_MESSAGE_BINARY_SERIALIZER_H

#include "WebXBinaryMessageSerializer.h"
#include <connector/message/WebXVoidMessage.h>

class WebXVoidMessageBinarySerializer : public WebXBinaryMessageSerializer<WebXVoidMessage> {
public:
    WebXVoidMessageBinarySerializer() {}
    virtual ~WebXVoidMessageBinarySerializer() {}

    // Structure:
    // Header: 16 bytes
    //   type: 4 bytes
    //   id: 4 bytes
    //   length: 4 bytes
    //   padding: 4 bytes
    // Content:
    //   commandId: 4 bytes
    virtual zmq::message_t * serialize(std::shared_ptr<WebXVoidMessage> message);
};


#endif //WEBX_VOID_MESSAGE_BINARY_SERIALIZER_H
