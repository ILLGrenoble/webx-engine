#include "WebXBinarySerializer.h"
#include <connector/message/WebXWindowsMessage.h>
#include <connector/message/WebXConnectionMessage.h>
#include <connector/message/WebXScreenMessage.h>
#include <connector/message/WebXImageMessage.h>
#include <connector/message/WebXSubImagesMessage.h>
#include <connector/instruction/WebXConnectInstruction.h>
#include <connector/instruction/WebXImageInstruction.h>
#include <connector/instruction/WebXScreenInstruction.h>
#include <connector/instruction/WebXWindowsInstruction.h>
#include <connector/instruction/WebXMouseInstruction.h>
#include <utils/WebXSize.h>
#include <utils/WebXBinaryBuffer.h>
#include <string>
#include <zmq.hpp>
#include <spdlog/spdlog.h>

WebXInstruction * WebXBinarySerializer::deserialize(void * instructionData, size_t instructionDataSize) {

    std::string instructionString = std::string(static_cast<char*>(instructionData), instructionDataSize);
    spdlog::info("instruction: {}", instructionString.c_str());

    // Convert to json
    nlohmann::json jInstruction = nlohmann::json::parse(instructionString);
    unsigned long type = jInstruction.at("type");
    unsigned long id = jInstruction.at("id");
    switch(type) {
        case 1: return new WebXConnectInstruction(id);
        case 2: return new WebXWindowsInstruction(id);
        case 3: {
            unsigned long windowId = jInstruction.at("windowId");
            return new WebXImageInstruction(id, windowId);
        }
        case 4: return new WebXScreenInstruction(id);
        case 5: {
            int x = jInstruction.at("x");
            int y = jInstruction.at("y");
            int buttonMask = jInstruction.at("buttonMask");
            return new WebXMouseInstruction(id, x, y, buttonMask);
        }
        default: return NULL;
    }
}

zmq::message_t * WebXBinarySerializer::serialize(WebXMessage * message) {

    zmq::message_t * messageData = NULL;

    if (message->type == WebXMessage::Type::Windows) {
        WebXWindowsMessage * windowsMessage = (WebXWindowsMessage *)message;

        // Structure:
        // Header: 16 bytes
        //   type: 4 bytes
        //   id: 4 bytes
        //   length: 4 bytes
        //   padding: 4 bytes
        // Content:
        //   commandId: 4 bytes
        //   # windows: 4 bytes
        //   Window: 24 bytes
        //      id: 4 bytes
        //      x: 4 bytes
        //      y: 4 bytes
        //      width: 4 bytes
        //      height: 4 bytes

        size_t dataSize = 16 + 8 + windowsMessage->windows.size() * 20;
        messageData = new zmq::message_t(dataSize);
        
        WebXBinaryBuffer buffer((unsigned char *)messageData->data(), dataSize, (uint32_t)message->type);
        buffer.write<uint32_t>(windowsMessage->commandId);
        buffer.write<uint32_t>(windowsMessage->windows.size());
        for (std::vector<WebXWindowProperties>::const_iterator it = windowsMessage->windows.begin(); it != windowsMessage->windows.end(); it++) {
            const WebXWindowProperties & window = *it;
            buffer.write<uint32_t>(window.id);
            buffer.write<int32_t>(window.x);
            buffer.write<int32_t>(window.y);
            buffer.write<int32_t>(window.width);
            buffer.write<int32_t>(window.height);
        }

    } else if (message->type == WebXMessage::Type::Connection) {
        WebXConnectionMessage * connectionMessage = (WebXConnectionMessage *)message;

        // Structure:
        // Header: 16 bytes
        //   type: 4 bytes
        //   id: 4 bytes
        //   length: 4 bytes
        //   padding: 4 bytes
        // Content:
        //   commandId: 4 bytes
        //   publisherPort: 4 bytes
        //   collectorPort: 4 bytes

        size_t dataSize = 16 + 12;
        messageData = new zmq::message_t(dataSize);

        WebXBinaryBuffer buffer((unsigned char *)messageData->data(), dataSize, (uint32_t)message->type);
        buffer.write<uint32_t>(connectionMessage->commandId);
        buffer.write<int32_t>(connectionMessage->publisherPort);
        buffer.write<int32_t>(connectionMessage->collectorPort);

    } else if (message->type == WebXMessage::Type::Screen) {
        WebXScreenMessage * screenMessage = (WebXScreenMessage *)message;

        // Structure:
        // Header: 16 bytes
        //   type: 4 bytes
        //   id: 4 bytes
        //   length: 4 bytes
        //   padding: 4 bytes
        // Content:
        //   commandId: 4 bytes
        //   screenWidth: 4 bytes
        //   screenHeight: 4 bytes

        size_t dataSize = 16 + 12;
        messageData = new zmq::message_t(dataSize);

        WebXBinaryBuffer buffer((unsigned char *)messageData->data(), dataSize, (uint32_t)message->type);
        buffer.write<uint32_t>(screenMessage->commandId);
        buffer.write<int32_t>(screenMessage->screenSize.width);
        buffer.write<int32_t>(screenMessage->screenSize.height);

    } else if (message->type == WebXMessage::Type::Image) {
        WebXImageMessage * imageMessage = (WebXImageMessage *)message;

        std::shared_ptr<WebXImage> image = imageMessage->image;
        size_t imageDataSize = 0;
        unsigned int depth = 0;
        char imageType[4] = "";
        if (image) {
            imageDataSize = image->getRawDataSize();
            depth = image->getDepth();
            strncpy(imageType, image->getFileExtension().c_str(), 4);
        }

        // Structure:
        // Header: 16 bytes
        //   type: 4 bytes
        //   id: 4 bytes
        //   length: 4 bytes
        //   padding: 4 bytes
        // Content:
        //   commandId: 4 bytes
        //   windowId: 4 bytes
        //   depth: 4 bytes
        //   imageType: 4 bytes (chars)
        //   imageDataLength: 4 bytes
        //   imageData: n bytes

        size_t dataSize = 16 + 20 + imageDataSize;
        messageData = new zmq::message_t(dataSize);

        WebXBinaryBuffer buffer((unsigned char *)messageData->data(), dataSize, (uint32_t)message->type);
        buffer.write<uint32_t>(imageMessage->commandId);
        buffer.write<uint32_t>(imageMessage->windowId);
        buffer.write<uint32_t>(depth);

        buffer.append((unsigned char *)imageType, 4);

        buffer.write<uint32_t>(imageDataSize);
        if (image) {
            buffer.append(image->getRawData(), image->getRawDataSize());
        }

    } else if (message->type == WebXMessage::Type::Subimages) {
        WebXSubImagesMessage * subImagesMessage = (WebXSubImagesMessage *)message;

        // Structure:
        // Header: 16 bytes
        //   type: 4 bytes
        //   id: 4 bytes
        //   length: 4 bytes
        //   padding: 4 bytes
        // Content:
        //   commandId: 4 bytes
        //   windowId: 4 bytes
        //   # subimages: 4 bytes
        //   Subimages:
        //     x: 4 bytes
        //     y: 4 bytes
        //     width: 4 bytes
        //     height: 4 bytes
        //     depth: 4 bytes
        //     imageType: 4 bytes (chars)
        //     imageDataLength: 4 bytes
        //     imageData: n bytes

        unsigned int nImages = subImagesMessage->images.size();
        size_t imageDataSize = 0;
        for (auto it = subImagesMessage->images.begin(); it != subImagesMessage->images.end(); it++) {
            const WebXSubImage & subImage = *it;
            imageDataSize += subImage.image->getRawDataSize();
            size_t alignmentOverflow = subImage.image->getRawDataSize() % 4;
            if (alignmentOverflow != 0) {
                size_t padding = 4 - alignmentOverflow;
                imageDataSize += padding;
            }
        }
        size_t dataSize = 16 + 12 + nImages * 28 + imageDataSize;
        messageData = new zmq::message_t(dataSize);

        WebXBinaryBuffer buffer((unsigned char *)messageData->data(), dataSize, (uint32_t)message->type);
        buffer.write<uint32_t>(subImagesMessage->commandId);
        buffer.write<uint32_t>(subImagesMessage->windowId);
        buffer.write<uint32_t>(nImages);

        for (auto it = subImagesMessage->images.begin(); it != subImagesMessage->images.end(); it++) {
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
    }

    if (messageData == NULL) {
        messageData = new zmq::message_t(0);
    }

    return messageData;
}
