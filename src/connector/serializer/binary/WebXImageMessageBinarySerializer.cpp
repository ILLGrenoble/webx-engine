#include "WebXImageMessageBinarySerializer.h"
#include <utils/WebXBinaryBuffer.h>
#include <image/WebXImage.h>

zmq::message_t * WebXImageMessageBinarySerializer::serialize(std::shared_ptr<WebXImageMessage> message) {
    std::shared_ptr<WebXImage> image = message->image;
    size_t imageDataSize = 0;
    unsigned int depth = 0;
    char imageType[4] = "";
    size_t alphaDataSize = 0;
    if (image) {
        imageDataSize = image->getRawDataSize();
        alphaDataSize = image->getAlphaDataSize();
        depth = image->getDepth();
        strncpy(imageType, image->getFileExtension().c_str(), 4);
    }

    size_t dataSize = 16 + 24 + imageDataSize + alphaDataSize;
    zmq::message_t * output= new zmq::message_t(dataSize);

    WebXBinaryBuffer buffer((unsigned char *)output->data(), dataSize, (uint32_t)message->type);
    buffer.write<uint32_t>(message->commandId);
    buffer.write<uint32_t>(message->windowId);
    buffer.write<uint32_t>(depth);

    buffer.append((unsigned char *)imageType, 4);

    buffer.write<uint32_t>(imageDataSize);
    buffer.write<uint32_t>(alphaDataSize);
    if (image) {
        buffer.append(image->getRawData(), image->getRawDataSize());

        if (alphaDataSize) {
            buffer.append(image->getAlphaData(), alphaDataSize);
        }
    }

    return output;
}