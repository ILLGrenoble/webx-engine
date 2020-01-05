#include "WebXMouseCursorMessageBinarySerializer.h"
#include <utils/WebXBinaryBuffer.h>

zmq::message_t * WebXMouseCursorMessageBinarySerializer::serialize(const WebXMouseCursorMessage * message) {
    std::shared_ptr<WebXImage> mouseCursorImage = message->mouseCursorImage;
    std::string cursorName = message->mouseCursorName;

    unsigned int nameLength = cursorName.size();

    size_t dataSize = 16 + 32 + mouseCursorImage->getRawDataSize() + nameLength;
    zmq::message_t * output= new zmq::message_t(dataSize);

    WebXBinaryBuffer buffer((unsigned char *)output->data(), dataSize, (uint32_t)message->type);
    buffer.write<uint32_t>(message->commandId);
    buffer.write<int32_t>(message->x);
    buffer.write<int32_t>(message->y);
    buffer.write<int32_t>(message->xhot);
    buffer.write<int32_t>(message->yhot);
    buffer.write<uint32_t>(cursorName.size());
    buffer.append((unsigned char *)cursorName.c_str(), nameLength);
    buffer.write<uint32_t>(mouseCursorImage->getRawDataSize());
    buffer.append(mouseCursorImage->getRawData(), mouseCursorImage->getRawDataSize());

    return output;
}