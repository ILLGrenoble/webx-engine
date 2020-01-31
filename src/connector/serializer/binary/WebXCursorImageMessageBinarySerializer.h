#ifndef WEBX_CURSOR_IMAGE_MESSAGE_BINARY_SERIALIZER_H
#define WEBX_CURSOR_IMAGE_MESSAGE_BINARY_SERIALIZER_H

#include "WebXBinaryMessageSerializer.h"
#include <connector/message/WebXCursorImageMessage.h>

class WebXCursorImageMessageBinarySerializer : public WebXBinaryMessageSerializer<WebXCursorImageMessage> {
public:
    WebXCursorImageMessageBinarySerializer() {}
    virtual ~WebXCursorImageMessageBinarySerializer() {}

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
    //   xHot: 4 bytes
    //   yHot: 4 bytes
    //   id: 4 bytes
    //   imageDataLength: 4 bytes
    //   imageData: n bytes
    virtual zmq::message_t * serialize(const WebXCursorImageMessage * message);
};


#endif //WEBX_CURSOR_IMAGE_MESSAGE_BINARY_SERIALIZER_H
