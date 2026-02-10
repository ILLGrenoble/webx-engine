#include "WebXInstructionDecoder.h"

#include <models/instruction/WebXMouseInstruction.h>
#include <models/instruction/WebXImageInstruction.h>
#include <models/instruction/WebXScreenInstruction.h>
#include <models/instruction/WebXWindowsInstruction.h>
#include <models/instruction/WebXKeyboardInstruction.h>
#include <models/instruction/WebXCursorImageInstruction.h>
#include <models/instruction/WebXMouseInstruction.h>
#include <models/instruction/WebXQualityInstruction.h>
#include <models/instruction/WebXPongInstruction.h>
#include <models/instruction/WebXDataAckInstruction.h>
#include <models/instruction/WebXClipboardInstruction.h>
#include <models/instruction/WebXShapeInstruction.h>
#include <models/instruction/WebXScreenResizeInstruction.h>
#include <models/instruction/WebXKeyboardLayoutInstruction.h>
#include <utils/WebXBinaryBuffer.h>

WebXInstructionDecoder::WebXInstructionDecoder() {
}

WebXInstructionDecoder::~WebXInstructionDecoder() {
}

std::shared_ptr<WebXInstruction> WebXInstructionDecoder::decode(const unsigned char * instructionData, size_t instructionDataSize) const {
    WebXBinaryBuffer buffer((unsigned char *)instructionData, instructionDataSize);
    
    // Ignore sessionId (this is filtered already by the ZMQ subscription)
    buffer.advanceReadOffset(16);

    uint32_t clientId = buffer.read<uint32_t>();
    uint32_t details = buffer.read<uint32_t>();
    uint32_t instructionId = buffer.read<uint32_t>();
    uint32_t dummy = buffer.read<uint32_t>();

    uint32_t type = details & ~0x80000000;

    if (type == WebXInstruction::Mouse) {
        return this->createMouseInstruction(clientId, instructionId, buffer);

    } else if (type == WebXInstruction::Keyboard) {
        return this->createKeyboardInstruction(clientId, instructionId, buffer);

    } else if (type == WebXInstruction::Cursor) {
        return this->createCursorImageInstruction(clientId, instructionId, buffer);

    } else if (type == WebXInstruction::Image) {
        return this->createImageInstruction(clientId, instructionId, buffer);

    } else if (type == WebXInstruction::Screen) {
        return this->createScreenInstruction(clientId, instructionId, buffer);

    } else if (type == WebXInstruction::Windows) {
        return this->createWindowsInstruction(clientId, instructionId, buffer);

    } else if (type == WebXInstruction::Quality) {
        return this->createQualityInstruction(clientId, instructionId, buffer);

    } else if (type == WebXInstruction::Pong) {
        return this->createPongInstruction(clientId, instructionId, buffer);

    } else if (type == WebXInstruction::DataAck) {
        return this->createDataAckInstruction(clientId, instructionId, buffer);

    } else if (type == WebXInstruction::Clipboard) {
        return this->createClipboardInstruction(clientId, instructionId, buffer);

    } else if (type == WebXInstruction::Shape) {
        return this->createShapeInstruction(clientId, instructionId, buffer);

    } else if (type == WebXInstruction::ScreenResize) {
        return this->createScreenResizeInstruction(clientId, instructionId, buffer);

    } else if (type == WebXInstruction::KeyboardLayout) {
        return this->createKeyboardLayoutInstruction(clientId, instructionId, buffer);
    }

    return nullptr;
}

inline std::shared_ptr<WebXInstruction> WebXInstructionDecoder::createMouseInstruction(uint32_t clientId, uint32_t instructionId, WebXBinaryBuffer & buffer) const {
    int32_t x = buffer.read<int32_t>();
    int32_t y = buffer.read<int32_t>();
    uint32_t buttonMask = buffer.read<uint32_t>();
    return std::make_shared<WebXMouseInstruction>(clientId, instructionId, x, y, buttonMask);
}

inline std::shared_ptr<WebXInstruction> WebXInstructionDecoder::createCursorImageInstruction(uint32_t clientId, uint32_t instructionId, WebXBinaryBuffer & buffer) const {
    int32_t cursorId = buffer.read<int32_t>();
    return std::make_shared<WebXCursorImageInstruction>(clientId, instructionId, cursorId);
}

inline std::shared_ptr<WebXInstruction> WebXInstructionDecoder::createImageInstruction(uint32_t clientId, uint32_t instructionId, WebXBinaryBuffer & buffer) const {
    uint32_t windowId = buffer.read<uint32_t>();
    return std::make_shared<WebXImageInstruction>(clientId, instructionId, windowId);
}

inline std::shared_ptr<WebXInstruction> WebXInstructionDecoder::createKeyboardInstruction(uint32_t clientId, uint32_t instructionId, WebXBinaryBuffer & buffer) const {
    uint32_t key = buffer.read<uint32_t>();
    bool pressed = buffer.read<uint32_t>() > 0;
    return std::make_shared<WebXKeyboardInstruction>(clientId, instructionId, key, pressed);
}

inline std::shared_ptr<WebXInstruction> WebXInstructionDecoder::createScreenInstruction(uint32_t clientId, uint32_t instructionId, WebXBinaryBuffer & buffer) const {
    return std::make_shared<WebXScreenInstruction>(clientId, instructionId);
}

inline std::shared_ptr<WebXInstruction> WebXInstructionDecoder::createWindowsInstruction(uint32_t clientId, uint32_t instructionId, WebXBinaryBuffer & buffer) const {
    return std::make_shared<WebXWindowsInstruction>(clientId, instructionId);
}

inline std::shared_ptr<WebXInstruction> WebXInstructionDecoder::createQualityInstruction(uint32_t clientId, uint32_t instructionId, WebXBinaryBuffer & buffer) const {
    uint32_t qualityIndex = buffer.read<uint32_t>();
    return std::make_shared<WebXQualityInstruction>(clientId, instructionId, qualityIndex);
}

inline std::shared_ptr<WebXInstruction> WebXInstructionDecoder::createPongInstruction(uint32_t clientId, uint32_t instructionId, WebXBinaryBuffer & buffer) const {
    uint64_t sendTimestampMs = buffer.read<uint64_t>();
    return std::make_shared<WebXPongInstruction>(clientId, instructionId, sendTimestampMs);
}

inline std::shared_ptr<WebXInstruction> WebXInstructionDecoder::createDataAckInstruction(uint32_t clientId, uint32_t instructionId, WebXBinaryBuffer & buffer) const {
    uint64_t sendTimestampMs = buffer.read<uint64_t>();
    uint64_t dataLength = buffer.read<uint32_t>();
    return std::make_shared<WebXDataAckInstruction>(clientId, instructionId, sendTimestampMs, dataLength);
}

inline std::shared_ptr<WebXInstruction> WebXInstructionDecoder::createClipboardInstruction(uint32_t clientId, uint32_t instructionId, WebXBinaryBuffer & buffer) const {
    uint32_t clipboardContentLength = buffer.read<uint32_t>();
    std::string clipboardContent = buffer.readSring(clipboardContentLength);
    return std::make_shared<WebXClipboardInstruction>(clientId, instructionId, clipboardContent);
}

inline std::shared_ptr<WebXInstruction> WebXInstructionDecoder::createShapeInstruction(uint32_t clientId, uint32_t instructionId, WebXBinaryBuffer & buffer) const {
    uint32_t windowId = buffer.read<uint32_t>();
    return std::make_shared<WebXShapeInstruction>(clientId, instructionId, windowId);
}

inline std::shared_ptr<WebXInstruction> WebXInstructionDecoder::createScreenResizeInstruction(uint32_t clientId, uint32_t instructionId, WebXBinaryBuffer & buffer) const {
    uint32_t width = buffer.read<int32_t>();
    uint32_t height = buffer.read<int32_t>();
    return std::make_shared<WebXScreenResizeInstruction>(clientId, instructionId, width, height);
}

inline std::shared_ptr<WebXInstruction> WebXInstructionDecoder::createKeyboardLayoutInstruction(uint32_t clientId, uint32_t instructionId, WebXBinaryBuffer & buffer) const {
    uint32_t keyboardLayoutLength = buffer.read<uint32_t>();
    std::string keyboardLayout = buffer.readSring(keyboardLayoutLength);
    return std::make_shared<WebXKeyboardLayoutInstruction>(clientId, instructionId, keyboardLayout);
}
    
