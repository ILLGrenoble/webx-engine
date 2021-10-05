#include "WebXConnectionMessageBinarySerializer.h"
#include <utils/WebXBinaryBuffer.h>

zmq::message_t * WebXConnectionMessageBinarySerializer::serialize(std::shared_ptr<WebXConnectionMessage> message) {
    size_t dataSize = 16 + 8;
    zmq::message_t * output= new zmq::message_t(dataSize);

    WebXBinaryBuffer buffer((unsigned char *)output->data(), dataSize, (uint32_t)message->type);
    buffer.write<uint32_t>(message->commandId);
    buffer.write<int32_t>(message->publisherPort);

    return output;
}