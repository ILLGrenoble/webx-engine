#include "WebXVoidMessageBinarySerializer.h"
#include <utils/WebXBinaryBuffer.h>

zmq::message_t * WebXVoidMessageBinarySerializer::serialize(std::shared_ptr<WebXVoidMessage> message) {
    size_t dataSize = 16 + 4;
    zmq::message_t * output= new zmq::message_t(dataSize);

    WebXBinaryBuffer buffer((unsigned char *)output->data(), dataSize, (uint32_t)message->type);
    buffer.write<uint32_t>(message->commandId);

    return output;
}