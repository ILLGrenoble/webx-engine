#ifndef WEBX_SUB_IMAGES_MESSAGE_BINARY_SERIALIZER_H
#define WEBX_SUB_IMAGES_MESSAGE_BINARY_SERIALIZER_H

#include <zmq.hpp>
#include "connector/message/WebXSubImagesMessage.h"
#include "connector/serializer/binary/WebXBinaryMessageSerializer.h"

class WebXSubImagesMessageBinarySerializer : public WebXBinaryMessageSerializer<WebXSubImagesMessage> {
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
    //   # subimages: 4 bytes
    //   Subimages:
    //     x: 4 bytes
    //     y: 4 bytes
    //     width: 4 bytes
    //     height: 4 bytes
    //     depth: 4 bytes
    //     imageType: 4 bytes (chars)
    //     imageDataLength: 4 bytes
    //     imageData: n bytes
    zmq::message_t * serialize(WebXSubImagesMessage * message);
};


#endif //WEBX_SUB_IMAGES_MESSAGE_BINARY_SERIALIZER_H
