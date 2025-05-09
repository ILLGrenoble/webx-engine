#include "WebXMessageEncoder.h"

#include <models/message/WebXMessage.h>
#include <models/message/WebXCursorImageMessage.h>
#include <models/message/WebXImageMessage.h>
#include <models/message/WebXMouseMessage.h>
#include <models/message/WebXScreenMessage.h>
#include <models/message/WebXSubImagesMessage.h>
#include <models/message/WebXWindowsMessage.h>
#include <models/message/WebXPingMessage.h>
#include <models/message/WebXDisconnectMessage.h>
#include <models/message/WebXQualityMessage.h>
#include <models/message/WebXClipboardMessage.h>
#include <utils/WebXBinaryBuffer.h>
#include <models/WebXSettings.h>
#include <zmq.hpp>

zmq::message_t * WebXMessageEncoder::encode(std::shared_ptr<WebXMessage> message) const {
    switch (message->type) {
        case WebXMessage::Windows: {
            auto windowsMessage = std::static_pointer_cast<WebXWindowsMessage>(message);
            return this->createWindowsMessage(windowsMessage);
        }
        case WebXMessage::Image: {
            auto imageMessage = std::static_pointer_cast<WebXImageMessage>(message);
            return this->createImageMessage(imageMessage);
        }
        case WebXMessage::Screen: {
            auto screenMessage = std::static_pointer_cast<WebXScreenMessage>(message);
            return this->createScreenMessage(screenMessage);
        }
        case WebXMessage::Subimages: {
            auto subImagesMessage = std::static_pointer_cast<WebXSubImagesMessage>(message);
            return this->createSubImagesMessage(subImagesMessage);
        }
        case WebXMessage::Mouse: {
            auto mouseMessage = std::static_pointer_cast<WebXMouseMessage>(message);
            return this->createMouseMessage(mouseMessage);
        }
        case WebXMessage::CursorImage: {
            auto cursorImageMessage = std::static_pointer_cast<WebXCursorImageMessage>(message);
            return this->createCursorImageMessage(cursorImageMessage);
        }
        case WebXMessage::Ping: {
            auto pingMessage = std::static_pointer_cast<WebXPingMessage>(message);
            return this->createPingMessage(pingMessage);
        }
        case WebXMessage::Disconnect: {
            auto disconnectMessage = std::static_pointer_cast<WebXDisconnectMessage>(message);
            return this->createDisconnectMessage(disconnectMessage);
        }
        case WebXMessage::Quality: {
            auto qualityMessage = std::static_pointer_cast<WebXQualityMessage>(message);
            return this->createQualityMessage(qualityMessage);
        }
        case WebXMessage::Clipboard: {
            auto clipboardMessage = std::static_pointer_cast<WebXClipboardMessage>(message);
            return this->createClipboardMessage(clipboardMessage);
        }

        default:
            return new zmq::message_t(0);
    }
}

zmq::message_t * WebXMessageEncoder::createCursorImageMessage(std::shared_ptr<WebXCursorImageMessage> message) const {
    std::shared_ptr<WebXImage> mouseCursorImage = message->mouseCursorImage;

    size_t dataSize = MESSAGE_HEADER_LENGTH + 28 + mouseCursorImage->getRawDataSize();
    zmq::message_t * output= new zmq::message_t(dataSize);

    WebXBinaryBuffer buffer((unsigned char *)output->data(), dataSize, this->_sessionId, message->clientIndexMask, (uint32_t)message->type);
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

zmq::message_t * WebXMessageEncoder::createImageMessage(std::shared_ptr<WebXImageMessage> message) const {
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

    size_t dataSize = MESSAGE_HEADER_LENGTH + 24 + imageDataSize + alphaDataSize;
    zmq::message_t * output= new zmq::message_t(dataSize);

    WebXBinaryBuffer buffer((unsigned char *)output->data(), dataSize, this->_sessionId, message->clientIndexMask, (uint32_t)message->type);
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

zmq::message_t * WebXMessageEncoder::createMouseMessage(std::shared_ptr<WebXMouseMessage> message) const {
    size_t dataSize = MESSAGE_HEADER_LENGTH + 16;
    zmq::message_t * output= new zmq::message_t(dataSize);

    WebXBinaryBuffer buffer((unsigned char *)output->data(), dataSize, this->_sessionId, message->clientIndexMask, (uint32_t)message->type);
    buffer.write<uint32_t>(message->commandId);
    buffer.write<int32_t>(message->x);
    buffer.write<int32_t>(message->y);
    buffer.write<uint32_t>(message->cursorId);

    return output;
}

zmq::message_t * WebXMessageEncoder::createScreenMessage(std::shared_ptr<WebXScreenMessage> message) const {
    size_t dataSize = MESSAGE_HEADER_LENGTH + 28;
    zmq::message_t * output= new zmq::message_t(dataSize);
    WebXBinaryBuffer buffer((unsigned char *) output->data(), dataSize, this->_sessionId, message->clientIndexMask, (uint32_t) message->type);
    buffer.write<uint32_t>(message->commandId);
    buffer.write<int32_t>(message->screenSize.width());
    buffer.write<int32_t>(message->screenSize.height());
    buffer.write<int32_t>(message->maxQualityIndex);
    buffer.write<int32_t>(message->engineVersion.major);
    buffer.write<int32_t>(message->engineVersion.minor);
    buffer.write<int32_t>(message->engineVersion.patch);
    return output;
}

zmq::message_t * WebXMessageEncoder::createSubImagesMessage(std::shared_ptr<WebXSubImagesMessage> message) const {
    unsigned int nImages = message->images.size();
    size_t imageDataSize = 0;
    size_t alphaDataSize = 0;
    for (const WebXSubImage & subImage : message->images) {
        size_t rawDataSize = subImage.image->getRawDataSize();
        size_t alphaDataSize = subImage.image->getAlphaDataSize();
        imageDataSize += rawDataSize + alphaDataSize;
        size_t alignmentOverflow = (rawDataSize + alphaDataSize) % 4;
        if (alignmentOverflow != 0) {
            size_t padding = 4 - alignmentOverflow;
            imageDataSize += padding;
        }
    }
    size_t dataSize = MESSAGE_HEADER_LENGTH + 12 + nImages * 32 + imageDataSize + alphaDataSize;
    zmq::message_t * output = new zmq::message_t(dataSize);

    WebXBinaryBuffer buffer((unsigned char *)output->data(), dataSize, this->_sessionId, message->clientIndexMask, (uint32_t)message->type);
    buffer.write<uint32_t>(message->commandId);
    buffer.write<uint32_t>(message->windowId);
    buffer.write<uint32_t>(nImages);

    for (const WebXSubImage & subImage : message->images) {
        buffer.write<int32_t>(subImage.imageRectangle.x());
        buffer.write<int32_t>(subImage.imageRectangle.y());
        buffer.write<int32_t>(subImage.imageRectangle.size().width());
        buffer.write<int32_t>(subImage.imageRectangle.size().height());
        buffer.write<uint32_t>(subImage.image->getDepth());

        char imageType[4] = "";
        strncpy(imageType, subImage.image->getFileExtension().c_str(), 4);
        buffer.append((unsigned char *)imageType, 4);

        buffer.write<uint32_t>(subImage.image->getRawDataSize());
        buffer.write<uint32_t>(subImage.image->getAlphaDataSize());
        buffer.append(subImage.image->getRawData(), subImage.image->getRawDataSize());
        if (subImage.image->getAlphaDataSize()) {
            buffer.append(subImage.image->getAlphaData(), subImage.image->getAlphaDataSize());
        }
    }

    return output;

}

zmq::message_t * WebXMessageEncoder::createWindowsMessage(std::shared_ptr<WebXWindowsMessage> message) const {
    size_t dataSize = MESSAGE_HEADER_LENGTH + 8 + message->windows.size() * 20;
    zmq::message_t * output = new zmq::message_t(dataSize);
    WebXBinaryBuffer buffer((unsigned char *)output->data(), dataSize, this->_sessionId, message->clientIndexMask, (uint32_t)message->type);
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

zmq::message_t * WebXMessageEncoder::createPingMessage(std::shared_ptr<WebXPingMessage> message) const {
    size_t dataSize = MESSAGE_HEADER_LENGTH;
    zmq::message_t * output = new zmq::message_t(dataSize);
    WebXBinaryBuffer buffer((unsigned char *)output->data(), dataSize, this->_sessionId, message->clientIndexMask, (uint32_t)message->type);

    return output;
}

zmq::message_t * WebXMessageEncoder::createDisconnectMessage(std::shared_ptr<WebXDisconnectMessage> message) const {
    size_t dataSize = MESSAGE_HEADER_LENGTH;
    zmq::message_t * output = new zmq::message_t(dataSize);
    WebXBinaryBuffer buffer((unsigned char *)output->data(), dataSize, this->_sessionId, message->clientIndexMask, (uint32_t)message->type);

    return output;
}

zmq::message_t * WebXMessageEncoder::createQualityMessage(std::shared_ptr<WebXQualityMessage> message) const {
    size_t dataSize = MESSAGE_HEADER_LENGTH + 20;
    zmq::message_t * output = new zmq::message_t(dataSize);
    WebXBinaryBuffer buffer((unsigned char *)output->data(), dataSize, this->_sessionId, message->clientIndexMask, (uint32_t)message->type);
    buffer.write<int32_t>(message->quality.index);
    buffer.write<float>(message->quality.imageFPS);
    buffer.write<float>(message->quality.rgbQuality);
    buffer.write<float>(message->quality.alphaQuality);
    buffer.write<float>(message->quality.maxMbps);

    return output;
}

zmq::message_t * WebXMessageEncoder::createClipboardMessage(std::shared_ptr<WebXClipboardMessage> message) const {
    const std::string & clipboardContent = message->clipboardContent;

    size_t dataSize = MESSAGE_HEADER_LENGTH + 4 + clipboardContent.size();
    zmq::message_t * output = new zmq::message_t(dataSize);
    WebXBinaryBuffer buffer((unsigned char *)output->data(), dataSize, this->_sessionId, message->clientIndexMask, (uint32_t)message->type);
    buffer.write<uint32_t>(clipboardContent.size());
    buffer.append((unsigned char *)clipboardContent.c_str(), clipboardContent.size());

    return output;
}
