#include "WebXJsonSerializer.h"
#include <connector/message/WebXWindowsMessage.h>
#include <connector/message/WebXConnectionMessage.h>
#include <connector/message/WebXImageMessage.h>
#include <connector/message/WebXSubImagesMessage.h>
#include <utils/WebXSize.h>
#include <string>
#include <zmq.hpp>
#include <base64/base64.h>

WebXInstruction WebXJsonSerializer::deserialize(void * instructionData, size_t instructionDataSize) {
    std::string instructionString = std::string(static_cast<char*>(instructionData), instructionDataSize);
    printf("instruction: %s\n", instructionString.c_str());

    // Convert to json
    nlohmann::json jinstruction = nlohmann::json::parse(instructionString);

    // Convert to instruction
    WebXInstruction instruction = jinstruction.get<WebXInstruction>();

    return instruction;
}

zmq::message_t * WebXJsonSerializer::serialize(WebXMessage * message) {
    nlohmann::json j;
    if (message->type == "windows") {
        WebXWindowsMessage * windowsMessage = (WebXWindowsMessage *)message;

        j = nlohmann::json{
            {"type", windowsMessage->type},
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

    } else if (message->type == "connection") {
        WebXConnectionMessage * connectionMessage = (WebXConnectionMessage *)message;

        j = nlohmann::json{
            {"type", connectionMessage->type},
            {"publisherPort", connectionMessage->publisherPort}, 
            {"collectorPort", connectionMessage->collectorPort}, 
            {"screenSize", {
                {"width", connectionMessage->screenSize.width},
                {"height", connectionMessage->screenSize.height}
            }}
        };

    } else if (message->type == "image") {
        WebXImageMessage * imageMessage = (WebXImageMessage *)message;

        j = nlohmann::json{
            {"type", imageMessage->type},
            {"windowId", imageMessage->windowId},
            {"depth", imageMessage->image->getDepth()},
            {"data", "data:image/" + imageMessage->image->getFileExtension() + ";base64," + base64_encode(imageMessage->image->getRawData(), imageMessage->image->getRawDataSize())}
        };

    } else if (message->type == "subimages") {
        WebXSubImagesMessage * subImagesMessage = (WebXSubImagesMessage *)message;

        j = nlohmann::json{
            {"type", subImagesMessage->type},
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


void from_json(const nlohmann::json& j, WebXInstruction & instruction) {
    try {j.at("type").get_to(instruction.type); } catch (...) {}
    try {j.at("stringPayload").get_to(instruction.stringPayload); } catch (...) {}
    try {j.at("numericPayload").get_to(instruction.numericPayload); } catch (...) {}
}