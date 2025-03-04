#include "WebXInstructionDecoder.h"

#include <instruction/WebXMouseInstruction.h>
#include <instruction/WebXImageInstruction.h>
#include <instruction/WebXScreenInstruction.h>
#include <instruction/WebXWindowsInstruction.h>
#include <instruction/WebXKeyboardInstruction.h>
#include <instruction/WebXCursorImageInstruction.h>
#include <instruction/WebXMouseInstruction.h>
#include <instruction/WebXQualityInstruction.h>
#include <instruction/WebXPongInstruction.h>
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
    uint32_t type = details & ~0x80000000;
    uint32_t instructionId = buffer.read<uint32_t>();

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
    return std::make_shared<WebXPongInstruction>(clientId, instructionId);
}
