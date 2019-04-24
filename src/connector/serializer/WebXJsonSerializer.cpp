#include "WebXJsonSerializer.h"
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
#include <string>
#include <zmq.hpp>
#include <base64/base64.h>
#include <spdlog/spdlog.h>

WebXInstruction * WebXJsonSerializer::deserialize(void * instructionData, size_t instructionDataSize) {

    std::string instructionString = std::string(static_cast<char*>(instructionData), instructionDataSize);
    spdlog::debug("Instruction: {}", instructionString.c_str());

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

zmq::message_t * WebXJsonSerializer::serialize(WebXMessage * message) {
    nlohmann::json j;
    if (message->type == WebXMessage::Type::Windows) {
        WebXWindowsMessage * windowsMessage = (WebXWindowsMessage *)message;

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
        WebXConnectionMessage * connectionMessage = (WebXConnectionMessage *)message;

        j = nlohmann::json{
            {"type", "connection"},
            {"commandId", connectionMessage->commandId},
            {"publisherPort", connectionMessage->publisherPort}, 
            {"collectorPort", connectionMessage->collectorPort}
        };

    } else if (message->type == WebXMessage::Type::Screen) {
        WebXScreenMessage * screenMessage = (WebXScreenMessage *)message;

        j = nlohmann::json{
            {"type", "screen"},
            {"commandId", screenMessage->commandId},
            {"screenSize", {
                {"width", screenMessage->screenSize.width},
                {"height", screenMessage->screenSize.height}
            }}
        };

    } else if (message->type == WebXMessage::Type::Image) {
        WebXImageMessage * imageMessage = (WebXImageMessage *)message;

        j = nlohmann::json{
            {"type", "image"},
            {"commandId", imageMessage->commandId},
            {"windowId", imageMessage->windowId},
            {"depth", (imageMessage->image == NULL) ? 0 : imageMessage->image->getDepth()},
            {"data", (imageMessage->image == NULL) ? "" : "data:image/" + imageMessage->image->getFileExtension() + ";base64," + base64_encode(imageMessage->image->getRawData(), imageMessage->image->getRawDataSize())}
        };

    } else if (message->type == WebXMessage::Type::Subimages) {
        WebXSubImagesMessage * subImagesMessage = (WebXSubImagesMessage *)message;

        j = nlohmann::json{
            {"type", "subimages"},
            {"commandId", subImagesMessage->commandId},
            {"windowId", subImagesMessage->windowId},
            {"subImages", {}}
        };
        for (auto it = subImagesMessage->images.begin(); it != subImagesMessage->images.end(); it++) {
            const WebXSubImage & subImage = *it;

            j["subImages"].push_back({
                {"x", subImage.imageRectangle.x},
                {"y", subImage.imageRectangle.y},
                {"width", subImage.imageRectangle.size.width},
                {"height", subImage.imageRectangle.size.height},
                {"depth", subImage.image->getDepth()},
                {"data", "data:image/" + subImage.image->getFileExtension() + ";base64," + base64_encode(subImage.image->getRawData(), subImage.image->getRawDataSize())}
            });
        }

    } else {
        return new zmq::message_t(0);
    }

    std::string messageDataString = j.dump();

    return new zmq::message_t(messageDataString.c_str(), messageDataString.size());
}


