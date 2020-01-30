#include <string>
#include <zmq.hpp>
#include <spdlog/spdlog.h>
#include "WebXBinarySerializer.h"
#include <connector/instruction/WebXConnectInstruction.h>
#include <connector/instruction/WebXImageInstruction.h>
#include <connector/instruction/WebXScreenInstruction.h>
#include <connector/instruction/WebXWindowsInstruction.h>
#include <connector/instruction/WebXMouseInstruction.h>
#include <connector/instruction/WebXKeyboardInstruction.h>
#include <connector/instruction/WebXCursorInstruction.h>
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

WebXInstruction * WebXBinarySerializer::deserialize(void * instructionData, size_t instructionDataSize) {

    std::string instructionString = std::string(static_cast<char*>(instructionData), instructionDataSize);
    spdlog::debug("instruction: {}", instructionString.c_str());

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
        case 6: {
            bool pressed = jInstruction.at("pressed");
            int key = jInstruction.at("key");
            return new WebXKeyboardInstruction(id, key, pressed);
        }
        case 7: return new WebXCursorInstruction(id);
        default: return NULL;
    }
}

zmq::message_t * WebXBinarySerializer::serialize(WebXMessage * message) {
    switch (message->type) {
        case WebXMessage::Connection: {
            auto connectionMessage = (WebXConnectionMessage *) message;
            WebXConnectionMessageBinarySerializer serializer;
            return serializer.serialize(connectionMessage);
        }
        case WebXMessage::Windows: {
            auto windowsMessage = (WebXWindowsMessage *) message;
            WebXWindowsMessageBinarySerializer serializer;
            return serializer.serialize(windowsMessage);
        }
        case WebXMessage::Image: {
            auto imageMessage = (WebXImageMessage *) message;
            WebXImageMessageBinarySerializer serializer;
            return serializer.serialize(imageMessage);
        }
        case WebXMessage::Screen: {
            auto screenMessage = (WebXScreenMessage *) message;
            WebXScreenMessageBinarySerializer serializer;
            return serializer.serialize(screenMessage);
        }
        case WebXMessage::Subimages: {
            auto subImagesMessage = (WebXSubImagesMessage *) message;
            WebXSubImagesMessageBinarySerializer serializer;
            return serializer.serialize(subImagesMessage);
        }
        case WebXMessage::Mouse: {
            auto mouseMessage = (WebXMouseMessage *) message;
            WebXMouseMessageBinarySerializer serializer;
            return serializer.serialize(mouseMessage);
        }
        case WebXMessage::CursorImage: {
            auto cursorImageMessage = (WebXCursorImageMessage *) message;
            WebXCursorImageMessageBinarySerializer serializer;
            return serializer.serialize(cursorImageMessage);
        }

        default:
            return new zmq::message_t(0);
    }

}

