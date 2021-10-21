#include "WebXInstructionDecoder.h"

#include <connector/instruction/WebXMouseInstruction.h>
#include <connector/instruction/WebXConnectInstruction.h>
#include <connector/instruction/WebXImageInstruction.h>
#include <connector/instruction/WebXScreenInstruction.h>
#include <connector/instruction/WebXWindowsInstruction.h>
#include <connector/instruction/WebXKeyboardInstruction.h>
#include <connector/instruction/WebXCursorImageInstruction.h>
#include <utils/WebXBinaryBuffer.h>

#include <spdlog/spdlog.h>


std::shared_ptr<WebXInstruction> WebXInstructionDecoder::deserialize(const unsigned char * instructionData, size_t instructionDataSize) {
    WebXBinaryBuffer buffer((unsigned char *)instructionData, instructionDataSize);

    uint32_t details = buffer.read<uint32_t>();
    uint32_t type = details & ~0x80000000;
    uint32_t instructionId = buffer.read<uint32_t>();

    if (type == WebXInstruction::Mouse) {
        return this->createMouseInstruction(instructionId, buffer);
    } else if (type == WebXInstruction::Keyboard) {
        return this->createKeyboardInstruction(instructionId, buffer);
    } else if (type == WebXInstruction::Cursor) {
        return this->createCursorImageInstruction(instructionId, buffer);
    } else if (type == WebXInstruction::Image) {
        return this->createImageInstruction(instructionId, buffer);
    } else if (type == WebXInstruction::Connect) {
        return this->createConnectInstruction(instructionId, buffer);
    } else if (type == WebXInstruction::Screen) {
        return this->createScreenInstruction(instructionId, buffer);
    } else if (type == WebXInstruction::Windows) {
        return this->createWindowsInstruction(instructionId, buffer);
    }

    return nullptr;
}

std::shared_ptr<WebXInstruction> WebXInstructionDecoder::createMouseInstruction(uint32_t instructionId, WebXBinaryBuffer & buffer) {
    int32_t x = buffer.read<int32_t>();
    int32_t y = buffer.read<int32_t>();
    uint32_t buttonMask = buffer.read<uint32_t>();
    return this->_webXMouseInstructionPool.getWithValues(instructionId, x, y, buttonMask);
}

std::shared_ptr<WebXInstruction> WebXInstructionDecoder::createCursorImageInstruction(uint32_t instructionId, WebXBinaryBuffer & buffer) {
    int32_t cursorId = buffer.read<int32_t>();
    return std::make_shared<WebXCursorImageInstruction>(instructionId, cursorId);
}

std::shared_ptr<WebXInstruction> WebXInstructionDecoder::createImageInstruction(uint32_t instructionId, WebXBinaryBuffer & buffer) {
    uint32_t windowId = buffer.read<uint32_t>();
    return std::make_shared<WebXImageInstruction>(instructionId, windowId);
}

std::shared_ptr<WebXInstruction> WebXInstructionDecoder::createKeyboardInstruction(uint32_t instructionId, WebXBinaryBuffer & buffer) {
    uint32_t key = buffer.read<uint32_t>();
    bool pressed = buffer.read<uint32_t>() > 0;
    return std::make_shared<WebXKeyboardInstruction>(instructionId, key, pressed);
}

std::shared_ptr<WebXInstruction> WebXInstructionDecoder::createScreenInstruction(uint32_t instructionId, WebXBinaryBuffer & buffer) {
    return std::make_shared<WebXScreenInstruction>(instructionId);
}

std::shared_ptr<WebXInstruction> WebXInstructionDecoder::createWindowsInstruction(uint32_t instructionId,WebXBinaryBuffer & buffer) {
    return std::make_shared<WebXWindowsInstruction>(instructionId);
}

std::shared_ptr<WebXInstruction> WebXInstructionDecoder::createConnectInstruction(uint32_t instructionId,WebXBinaryBuffer & buffer) {
    return std::make_shared<WebXConnectInstruction>(instructionId);
}
