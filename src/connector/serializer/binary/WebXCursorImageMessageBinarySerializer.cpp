#include "WebXCursorImageMessageBinarySerializer.h"
#include <utils/WebXBinaryBuffer.h>

zmq::message_t * WebXCursorImageMessageBinarySerializer::serialize(std::shared_ptr<WebXCursorImageMessage> message) {
    std::shared_ptr<WebXImage> mouseCursorImage = message->mouseCursorImage;

    size_t dataSize = 16 + 28 + mouseCursorImage->getRawDataSize();
    zmq::message_t * output= new zmq::message_t(dataSize);

    WebXBinaryBuffer buffer((unsigned char *)output->data(), dataSize, (uint32_t)message->type);
    buffer.write<uint32_t>(message->commandId);
    buffer.write<int32_t>(message->x);
    buffer.write<int32_t>(message->y);
    buffer.write<int32_t>(message->xhot);
    buffer.write<int32_t>(message->yhot);
    buffer.write<uint32_t>(message->cursorId);
    buffer.write<uint32_t>(mouseCursorImage->getRawDataSize());
    buffer.append(mouseCursorImage->getRawData(), mouseCursorImage->getRawDataSize());

    return output;
}