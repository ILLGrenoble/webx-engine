#include "WebXJsonSerializer.h"
#include <connector/message/WebXWindowsMessage.h>
#include <connector/message/WebXConnectionMessage.h>
#include <connector/message/WebXScreenMessage.h>
#include <connector/message/WebXImageMessage.h>
#include <connector/message/WebXSubImagesMessage.h>
#include <connector/message/WebXMouseMessage.h>
#include <connector/message/WebXCursorImageMessage.h>
#include <connector/message/WebXVoidMessage.h>
#include <connector/instruction/WebXConnectInstruction.h>
#include <connector/instruction/WebXImageInstruction.h>
#include <connector/instruction/WebXScreenInstruction.h>
#include <connector/instruction/WebXWindowsInstruction.h>
#include <connector/instruction/WebXMouseInstruction.h>
#include <connector/instruction/WebXKeyboardInstruction.h>
#include <connector/instruction/WebXCursorImageInstruction.h>
#include <utils/WebXSize.h>
#include <string>
#include <zmq.hpp>
#include <base64/base64.h>
#include <spdlog/spdlog.h>

std::shared_ptr<WebXInstruction> WebXJsonSerializer::deserialize(void * instructionData, size_t instructionDataSize) {

    std::string instructionString = std::string(static_cast<char*>(instructionData), instructionDataSize);
    spdlog::trace("instruction: {}", instructionString.c_str());

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
        auto instruction = this->_webXMouseInstructionPool.getWithValues(id, x, y, buttonMask);
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

zmq::message_t * WebXJsonSerializer::serialize(std::shared_ptr<WebXMessage> message) {
    nlohmann::json j;
    if (message->type == WebXMessage::Type::Windows) {
        auto windowsMessage = std::static_pointer_cast<WebXWindowsMessage>(message);

        j = nlohmann::json{
            {"type", "windows"},
            {"commandId", windowsMessage->commandId},
            {"windows", {}}
        };
        for (std::vector<WebXWindowProperties>::const_iterator it = windowsMessage->windows.begin(); it != windowsMessage->windows.end(); it++) {
            const WebXWindowProperties & window = *it;
            j["windows"].push_back({
                {"id", window.id}, 
                {"x", window.x},
                {"y", window.y},
                {"width", window.width},
                {"height", window.height}
            });
        }

    } else if (message->type == WebXMessage::Type::Connection) {
        auto connectionMessage = std::static_pointer_cast<WebXConnectionMessage>(message);

        j = nlohmann::json{
            {"type", "connection"},
            {"commandId", connectionMessage->commandId},
            {"publisherPort", connectionMessage->publisherPort}
        };

    } else if (message->type == WebXMessage::Type::Screen) {
        auto screenMessage = std::static_pointer_cast<WebXScreenMessage>(message);

        j = nlohmann::json{
            {"type", "screen"},
            {"commandId", screenMessage->commandId},
            {"screenSize", {
                {"width", screenMessage->screenSize.width},
                {"height", screenMessage->screenSize.height}
            }}
        };

    } else if (message->type == WebXMessage::Type::Image) {
        auto imageMessage = std::static_pointer_cast<WebXImageMessage>(message);

        j = nlohmann::json{
            {"type", "image"},
            {"commandId", imageMessage->commandId},
            {"windowId", imageMessage->windowId},
            {"depth", (imageMessage->image == NULL) ? 0 : imageMessage->image->getDepth()},
            {"data", (imageMessage->image == NULL) ? "" : "data:image/" + imageMessage->image->getFileExtension() + ";base64," + base64_encode(imageMessage->image->getRawData(), imageMessage->image->getRawDataSize())}
        };

    } else if (message->type == WebXMessage::Type::Subimages) {
        auto subImagesMessage = std::static_pointer_cast<WebXSubImagesMessage>(message);

        j = nlohmann::json{
            {"type",      "subimages"},
            {"commandId", subImagesMessage->commandId},
            {"windowId",  subImagesMessage->windowId},
            {"subImages", {}}
        };
        for (auto it = subImagesMessage->images.begin(); it != subImagesMessage->images.end(); it++) {
            const WebXSubImage &subImage = *it;

            j["subImages"].push_back({
                {"x",      subImage.imageRectangle.x},
                {"y",      subImage.imageRectangle.y},
                {"width",  subImage.imageRectangle.size.width},
                {"height", subImage.imageRectangle.size.height},
                {"depth",  subImage.image->getDepth()},
                {"data",   "data:image/" + subImage.image->getFileExtension() + ";base64," + base64_encode(subImage.image->getRawData(), subImage.image->getRawDataSize())}
            });
        }
        
    } else if (message->type == WebXMessage::Type::Mouse) {
        auto cursorMessage = std::static_pointer_cast<WebXMouseMessage>(message);

        j = nlohmann::json{
            {"type", "mouse"},
            {"commandId", cursorMessage->commandId},
            {"x", cursorMessage->x},
            {"y", cursorMessage->y},
            {"cursorId", cursorMessage->cursorId}
        };

    } else if (message->type == WebXMessage::Type::CursorImage) {
        auto cursorMessage = std::static_pointer_cast<WebXCursorImageMessage>(message);
        auto cursorImage  = cursorMessage->mouseCursorImage;

        j = nlohmann::json{
            {"type", "cursorimage"},
            {"commandId", cursorMessage->commandId},
            {"x", cursorMessage->x},
            {"y", cursorMessage->y},
            {"xHot", cursorMessage->xhot},
            {"yHot", cursorMessage->yhot},
            {"cursorId", cursorMessage->cursorId},
            {"data",   "data:image/" + cursorImage->getFileExtension() + ";base64," + base64_encode(cursorImage->getRawData(), cursorImage->getRawDataSize())}
        };

    } else if (message->type == WebXMessage::Type::Void) {
        auto voidMessage = std::static_pointer_cast<WebXVoidMessage>(message);

        j = nlohmann::json{
            {"type", "void"},
            {"commandId", voidMessage->commandId}
        };

    } else {
        return new zmq::message_t(0);
    }

    std::string messageDataString = j.dump();

    return new zmq::message_t(messageDataString.c_str(), messageDataString.size());
}


