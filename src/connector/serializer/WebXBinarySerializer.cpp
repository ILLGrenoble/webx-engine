#include <string>
#include <zmq.hpp>
#include <spdlog/spdlog.h>
#include "WebXBinarySerializer.h"
#include <connector/instruction/WebXConnectInstruction.h>
#include <connector/instruction/WebXImageInstruction.h>
#include <connector/instruction/WebXScreenInstruction.h>
#include <connector/instruction/WebXWindowsInstruction.h>
#include <connector/instruction/WebXKeyboardInstruction.h>
#include <connector/instruction/WebXCursorImageInstruction.h>
#include <connector/message/WebXSubImagesMessage.h>
#include <connector/message/WebXImageMessage.h>
#include <connector/message/WebXScreenMessage.h>
#include <connector/message/WebXWindowsMessage.h>
#include <connector/message/WebXMouseMessage.h>
#include <connector/message/WebXCursorImageMessage.h>
#include <connector/message/WebXConnectionMessage.h>
#include <connector/message/WebXVoidMessage.h>
#include "binary/WebXScreenMessageBinarySerializer.h"
#include "binary/WebXWindowsMessageBinarySerializer.h"
#include "binary/WebXSubImagesMessageBinarySerializer.h"
#include "binary/WebXImageMessageBinarySerializer.h"
#include "binary/WebXConnectionMessageBinarySerializer.h"
#include "binary/WebXMouseMessageBinarySerializer.h"
#include "binary/WebXCursorImageMessageBinarySerializer.h"
#include "binary/WebXVoidMessageBinarySerializer.h"
#include "utils/WebXBinaryBuffer.h"


WebXBinarySerializer::WebXBinarySerializer() :
     _screenMessageSerializer(new WebXScreenMessageBinarySerializer()),
     _windowsMessageSerializer(new WebXWindowsMessageBinarySerializer()),
     _subImagesMessageSerializer(new WebXSubImagesMessageBinarySerializer()),
     _imageMessageSerializer(new WebXImageMessageBinarySerializer()),
     _connectionMessageSerializer(new WebXConnectionMessageBinarySerializer()),
     _voidMessageSerializer(new WebXVoidMessageBinarySerializer()),
     _mouseMessageSerializer(new WebXMouseMessageBinarySerializer()),
     _cursorImageMessageSerializer(new WebXCursorImageMessageBinarySerializer()),
     _instructionDecoder(new WebXInstructionDecoder()) {
}

WebXBinarySerializer::~WebXBinarySerializer() {

}

std::shared_ptr<WebXInstruction> WebXBinarySerializer::deserialize(const unsigned char * instructionData, size_t instructionDataSize) {
    return this->_instructionDecoder->deserialize(instructionData, instructionDataSize);
}

zmq::message_t * WebXBinarySerializer::serialize(std::shared_ptr<WebXMessage> message) {
    switch (message->type) {
        case WebXMessage::Connection: {
            auto connectionMessage = std::static_pointer_cast<WebXConnectionMessage>(message);
            return this->_connectionMessageSerializer->serialize(connectionMessage);
        }
        case WebXMessage::Windows: {
            auto windowsMessage = std::static_pointer_cast<WebXWindowsMessage>(message);
            return this->_windowsMessageSerializer->serialize(windowsMessage);
        }
        case WebXMessage::Image: {
            auto imageMessage = std::static_pointer_cast<WebXImageMessage>(message);
            return this->_imageMessageSerializer->serialize(imageMessage);
        }
        case WebXMessage::Screen: {
            auto screenMessage = std::static_pointer_cast<WebXScreenMessage>(message);
            return this->_screenMessageSerializer->serialize(screenMessage);
        }
        case WebXMessage::Subimages: {
            auto subImagesMessage = std::static_pointer_cast<WebXSubImagesMessage>(message);
            return this->_subImagesMessageSerializer->serialize(subImagesMessage);
        }
        case WebXMessage::Mouse: {
            auto mouseMessage = std::static_pointer_cast<WebXMouseMessage>(message);
            return this->_mouseMessageSerializer->serialize(mouseMessage);
        }
        case WebXMessage::CursorImage: {
            auto cursorImageMessage = std::static_pointer_cast<WebXCursorImageMessage>(message);
            return this->_cursorImageMessageSerializer->serialize(cursorImageMessage);
        }
        case WebXMessage::Void: {
            auto voidMessage = std::static_pointer_cast<WebXVoidMessage>(message);
            return this->_voidMessageSerializer->serialize(voidMessage);
        }

        default:
            return new zmq::message_t(0);
    }

}

