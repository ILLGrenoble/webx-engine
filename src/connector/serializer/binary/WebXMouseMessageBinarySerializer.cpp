#include "WebXMouseMessageBinarySerializer.h"
#include <utils/WebXBinaryBuffer.h>

zmq::message_t * WebXMouseMessageBinarySerializer::serialize(std::shared_ptr<WebXMouseMessage> message) {
    size_t dataSize = 32;
    zmq::message_t * output= new zmq::message_t(dataSize);

    WebXBinaryBuffer buffer((unsigned char *)output->data(), dataSize, (uint32_t)message->type);
    buffer.write<uint32_t>(message->commandId);
    buffer.write<int32_t>(message->x);
    buffer.write<int32_t>(message->y);
    buffer.write<uint32_t>(message->cursorId);

    return output;
}