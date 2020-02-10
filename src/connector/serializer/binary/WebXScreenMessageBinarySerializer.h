#ifndef WEBX_SCREEN_MESSAGE_BINARY_SERIALIZER_H
#define WEBX_SCREEN_MESSAGE_BINARY_SERIALIZER_H

#include "WebXBinaryMessageSerializer.h"
#include <connector/message/WebXScreenMessage.h>

class WebXScreenMessageBinarySerializer : public WebXBinaryMessageSerializer<WebXScreenMessage> {
public:
    WebXScreenMessageBinarySerializer() {}
    virtual ~WebXScreenMessageBinarySerializer() {}

    // Structure:
    // Header: 16 bytes
    //   type: 4 bytes
    //   id: 4 bytes
    //   length: 4 bytes
    //   padding: 4 bytes
    // Content:
    //   commandId: 4 bytes
    //   screenWidth: 4 bytes
    //   screenHeight: 4 bytes
    virtual zmq::message_t * serialize(std::shared_ptr<WebXScreenMessage> message);
};


#endif //WEBX_SCREEN_MESSAGE_BINARY_SERIALIZER_H
