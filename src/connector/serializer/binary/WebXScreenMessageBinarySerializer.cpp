#include <zmq.hpp>
#include "WebXScreenMessageBinarySerializer.h"
#include "utils/WebXBinaryBuffer.h"

zmq::message_t * WebXScreenMessageBinarySerializer::serialize(WebXScreenMessage * message) {
    size_t dataSize = 16 + 12;
    zmq::message_t * output= new zmq::message_t(dataSize);
    WebXBinaryBuffer buffer((unsigned char *) output->data(), dataSize, (uint32_t) message->type);
    buffer.write<uint32_t>(message->commandId);
    buffer.write<int32_t>(message->screenSize.width);
    buffer.write<int32_t>(message->screenSize.height);
    return output;
}