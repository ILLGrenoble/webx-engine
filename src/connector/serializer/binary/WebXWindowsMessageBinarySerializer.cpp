#include "WebXWindowsMessageBinarySerializer.h"
#include <utils/WebXBinaryBuffer.h>

zmq::message_t * WebXWindowsMessageBinarySerializer::serialize(std::shared_ptr<WebXWindowsMessage> message) {
    size_t dataSize = 16 + 8 + message->windows.size() * 20;
    zmq::message_t * output = new zmq::message_t(dataSize);
    WebXBinaryBuffer buffer((unsigned char *)output->data(), dataSize, (uint32_t)message->type);
    buffer.write<uint32_t>(message->commandId);
    buffer.write<uint32_t>(message->windows.size());
    for (std::vector<WebXWindowProperties>::const_iterator it = message->windows.begin(); it != message->windows.end(); it++) {
        const WebXWindowProperties & window = *it;
        buffer.write<uint32_t>(window.id);
        buffer.write<int32_t>(window.x);
        buffer.write<int32_t>(window.y);
        buffer.write<int32_t>(window.width);
        buffer.write<int32_t>(window.height);
    }

    return output;
}