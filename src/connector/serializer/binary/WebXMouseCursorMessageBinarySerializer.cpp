#include <zmq.hpp>
#include "WebXMouseCursorMessageBinarySerializer.h"
#include "utils/WebXBinaryBuffer.h"
#include "input/cursor/WebXMouseCursor.h"
#include "input/cursor/WebXMouseCursorImage.h"

zmq::message_t * WebXMouseCursorMessageBinarySerializer::serialize(const WebXMouseCursorMessage * message) {
    WebXMouseCursor * mouseCursor = message->mouseCursor;
    std::shared_ptr<WebXMouseCursorImage> mouseCursorImage = mouseCursor->getImage();
    size_t dataSize = 16 + 8 + mouseCursorImage->getRawDataSize();
    zmq::message_t * output= new zmq::message_t(dataSize);

    WebXBinaryBuffer buffer((unsigned char *)output->data(), dataSize, (uint32_t)message->type);
    buffer.write<uint32_t>(message->commandId);
    buffer.write<uint32_t>(mouseCursorImage->getRawDataSize());
    buffer.append(mouseCursorImage->getRawData(), mouseCursorImage->getRawDataSize());

    return output;
}