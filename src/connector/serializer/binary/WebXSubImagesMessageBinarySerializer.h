#ifndef WEBX_SUB_IMAGES_MESSAGE_BINARY_SERIALIZER_H
#define WEBX_SUB_IMAGES_MESSAGE_BINARY_SERIALIZER_H

#include "WebXBinaryMessageSerializer.h"
#include <connector/message/WebXSubImagesMessage.h>

class WebXSubImagesMessageBinarySerializer : public WebXBinaryMessageSerializer<WebXSubImagesMessage> {
public:
    WebXSubImagesMessageBinarySerializer() {}
    virtual ~WebXSubImagesMessageBinarySerializer() {}

    // Structure:
    // Header: 16 bytes
    //   type: 4 bytes
    //   id: 4 bytes
    //   length: 4 bytes
    //   padding: 4 bytes
    // Content:
    //   commandId: 4 bytes
    //   windowId: 4 bytes
    //   # subimages: 4 bytes
    //   Subimages:
    //     x: 4 bytes
    //     y: 4 bytes
    //     width: 4 bytes
    //     height: 4 bytes
    //     depth: 4 bytes
    //     imageType: 4 bytes (chars)
    //     imageDataLength: 4 bytes
    //     alphaDataLength: 4 bytes (0 if no alpha data)
    //     imageData: n bytes
    //     alphaData: n bytes (optional)
    virtual zmq::message_t * serialize(std::shared_ptr<WebXSubImagesMessage> message);
};


#endif //WEBX_SUB_IMAGES_MESSAGE_BINARY_SERIALIZER_H
