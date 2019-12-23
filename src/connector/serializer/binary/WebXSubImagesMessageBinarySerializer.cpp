#include "WebXSubImagesMessageBinarySerializer.h"
#include <utils/WebXBinaryBuffer.h>
#include <image/WebXSubImage.h>
#include <image/WebXImage.h>

zmq::message_t * WebXSubImagesMessageBinarySerializer::serialize(const WebXSubImagesMessage * message) {
    unsigned int nImages = message->images.size();
    size_t imageDataSize = 0;
    for (auto it = message->images.begin(); it != message->images.end(); it++) {
        const WebXSubImage & subImage = *it;
        imageDataSize += subImage.image->getRawDataSize();
        size_t alignmentOverflow = subImage.image->getRawDataSize() % 4;
        if (alignmentOverflow != 0) {
            size_t padding = 4 - alignmentOverflow;
            imageDataSize += padding;
        }
    }
    size_t dataSize = 16 + 12 + nImages * 28 + imageDataSize;
    zmq::message_t * output = new zmq::message_t(dataSize);

    WebXBinaryBuffer buffer((unsigned char *)output->data(), dataSize, (uint32_t)message->type);
    buffer.write<uint32_t>(message->commandId);
    buffer.write<uint32_t>(message->windowId);
    buffer.write<uint32_t>(nImages);

    for (auto it = message->images.begin(); it != message->images.end(); it++) {
        const WebXSubImage & subImage = *it;

        buffer.write<int32_t>(subImage.imageRectangle.x);
        buffer.write<int32_t>(subImage.imageRectangle.y);
        buffer.write<int32_t>(subImage.imageRectangle.size.width);
        buffer.write<int32_t>(subImage.imageRectangle.size.height);
        buffer.write<uint32_t>(subImage.image->getDepth());

        char imageType[4] = "";
        strncpy(imageType, subImage.image->getFileExtension().c_str(), 4);
        buffer.append((unsigned char *)imageType, 4);

        buffer.write<uint32_t>(subImage.image->getRawDataSize());
        buffer.append(subImage.image->getRawData(), subImage.image->getRawDataSize());
    }

    return output;

}