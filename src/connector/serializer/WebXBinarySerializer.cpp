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
#include "binary/WebXScreenMessageBinarySerializer.h"
#include "binary/WebXWindowsMessageBinarySerializer.h"
#include "binary/WebXSubImagesMessageBinarySerializer.h"
#include "binary/WebXImageMessageBinarySerializer.h"
#include "binary/WebXConnectionMessageBinarySerializer.h"
#include "binary/WebXMouseMessageBinarySerializer.h"
#include "binary/WebXCursorImageMessageBinarySerializer.h"


WebXBinarySerializer::WebXBinarySerializer() :
    WebXSerializer("binary"),
     _screenMessageSerializer(new WebXScreenMessageBinarySerializer()),
     _windowsMessageSerializer(new WebXWindowsMessageBinarySerializer()),
     _subImagesMessageSerializer(new WebXSubImagesMessageBinarySerializer()),
     _imageMessageSerializer(new WebXImageMessageBinarySerializer()),
     _connectionMessageSerializer(new WebXConnectionMessageBinarySerializer()),
     _mouseMessageSerializer(new WebXMouseMessageBinarySerializer()),
     _cursorImageMessageSerializer(new WebXCursorImageMessageBinarySerializer()) {
}

WebXBinarySerializer::~WebXBinarySerializer() {

}

std::shared_ptr<WebXInstruction> WebXBinarySerializer::deserialize(void * instructionData, size_t instructionDataSize) {

    std::string instructionString = std::string(static_cast<char*>(instructionData), instructionDataSize);
    spdlog::debug("instruction: {}", instructionString.c_str());

    // Convert to json
    nlohmann::json jInstruction = nlohmann::json::parse(instructionString);
    unsigned long type = jInstruction.at("type");
    unsigned long id = jInstruction.at("id");
    if (type == 1) {
        auto instruction = std::make_shared<WebXConnectInstruction>(id);
        return instruction;

    } else if (type == 2) {
        auto instruction = std::make_shared<WebXWindowsInstruction>(id);
        return instruction;

    } else if (type == 3) {
        unsigned long windowId = jInstruction.at("windowId");
        auto instruction = std::make_shared<WebXImageInstruction>(id, windowId);
        return instruction;

    } else if (type == 4) {
        auto instruction = std::make_shared<WebXScreenInstruction>(id);
        return instruction;

    } else if (type == 5) {
        int x = jInstruction.at("x");
        int y = jInstruction.at("y");
        int buttonMask = jInstruction.at("buttonMask");
        auto instruction = this->_webXMouseInstructionPool.get();
        instruction->set(id, x, y, buttonMask);
        return instruction;

    } else if (type == 6) {
        bool pressed = jInstruction.at("pressed");
        int key = jInstruction.at("key");
        auto instruction = std::make_shared<WebXKeyboardInstruction>(id, key, pressed);
        return instruction;

    } else if (type == 7) {
        uint32_t cursorId = jInstruction.at("cursorId");
        auto instruction = std::make_shared<WebXCursorImageInstruction>(id ,cursorId);
        return instruction;
    
    } else {
        return nullptr;
    }
}

zmq::message_t * WebXBinarySerializer::serialize(WebXMessage * message) {
    switch (message->type) {
        case WebXMessage::Connection: {
            auto connectionMessage = (WebXConnectionMessage *) message;
            return this->_connectionMessageSerializer->serialize(connectionMessage);
        }
        case WebXMessage::Windows: {
            auto windowsMessage = (WebXWindowsMessage *) message;
            return this->_windowsMessageSerializer->serialize(windowsMessage);
        }
        case WebXMessage::Image: {
            auto imageMessage = (WebXImageMessage *) message;
            return this->_imageMessageSerializer->serialize(imageMessage);
        }
        case WebXMessage::Screen: {
            auto screenMessage = (WebXScreenMessage *) message;
            return this->_screenMessageSerializer->serialize(screenMessage);
        }
        case WebXMessage::Subimages: {
            auto subImagesMessage = (WebXSubImagesMessage *) message;
            return this->_subImagesMessageSerializer->serialize(subImagesMessage);
        }
        case WebXMessage::Mouse: {
            auto mouseMessage = (WebXMouseMessage *) message;
            return this->_mouseMessageSerializer->serialize(mouseMessage);
        }
        case WebXMessage::CursorImage: {
            auto cursorImageMessage = (WebXCursorImageMessage *) message;
            return this->_cursorImageMessageSerializer->serialize(cursorImageMessage);
        }

        default:
            return new zmq::message_t(0);
    }

}

