#ifndef WEBX_MOUSE_CURSOR_MESSAGE_BINARY_SERIALIZER_H
#define WEBX_MOUSE_CURSOR_MESSAGE_BINARY_SERIALIZER_H

#include "WebXBinaryMessageSerializer.h"
#include <connector/message/WebXMouseCursorMessage.h>

class WebXMouseCursorMessageBinarySerializer : public WebXBinaryMessageSerializer<WebXMouseCursorMessage> {
public:
    WebXMouseCursorMessageBinarySerializer() {}
    virtual ~WebXMouseCursorMessageBinarySerializer() {}

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
    //   imageNameLength: 4 bytes
    //   imageName: n bytes
    //   imageDataLength: 4 bytes
    //   imageData: n bytes
    virtual zmq::message_t * serialize(const WebXMouseCursorMessage * message);
};


#endif //WEBX_MOUSE_CURSOR_MESSAGE_BINARY_SERIALIZER_H
