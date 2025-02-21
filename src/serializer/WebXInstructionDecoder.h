
#ifndef WEBX_INSTRUCTION_DECODER_H
#define WEBX_INSTRUCTION_DECODER_H

#include <memory>

class WebXBinaryBuffer;
class WebXInstruction;

class WebXInstructionDecoder {
public:
    WebXInstructionDecoder();
    ~WebXInstructionDecoder();

    std::shared_ptr<WebXInstruction> decode(const unsigned char * instructionData, size_t instructionDataSize) const;

private:
    /**
     * Create a new mouse instruction
     * Structure:
     *   Header: 16 bytes
     *    type: 4 bytes
     *    id: 4 bytes
     *    length: 4 bytes
     *    padding: 4 bytes
     *   Content: 4 bytes
     *    x: 4 bytes
     *    y: 4 bytes
     *    buttonMask: 4 bytes
     */
    std::shared_ptr<WebXInstruction> createMouseInstruction(uint32_t instructionId, WebXBinaryBuffer & buffer) const;

    /**
     * Create a new cursor image instruction
     * Structure:
     *   Header: 16 bytes
     *    type: 4 bytes
     *    id: 4 bytes
     *    length: 4 bytes
     *    padding: 4 bytes
     *   Content: 4 bytes
     *    cursorId: 4 bytes
     */
    std::shared_ptr<WebXInstruction> createCursorImageInstruction(uint32_t instructionId, WebXBinaryBuffer & buffer) const;

    /**
     * Create a new image instruction
     * Structure:
     *   Header: 16 bytes
     *    type: 4 bytes
     *    id: 4 bytes
     *    length: 4 bytes
     *    padding: 4 bytes
     *   Content: 4 bytes
     *    windowId: 4 bytes
     */
    std::shared_ptr<WebXInstruction> createImageInstruction(uint32_t instructionId, WebXBinaryBuffer & buffer) const;

    /**
     * Create a keyboard instruction
     * Structure:
     *   Header: 16 bytes
     *    type: 4 bytes
     *    id: 4 bytes
     *    length: 4 bytes
     *    padding: 4 bytes
     *   Content: 8 bytes
     *    key (the keyboard key code): 4 bytes
     *    pressed: 4 bytes
     */
    std::shared_ptr<WebXInstruction> createKeyboardInstruction(uint32_t instructionId, WebXBinaryBuffer & buffer) const;

    /**
     * Create a screen instruction
     * Structure:
     *   Header: 16 bytes
     *    type: 4 bytes
     *    id: 4 bytes
     *    length: 4 bytes
     *    padding: 4 bytes
     */
    std::shared_ptr<WebXInstruction> createScreenInstruction(uint32_t instructionId, WebXBinaryBuffer & buffer) const;

    /**
     * Create a windows instruction
     * Structure:
     *   Header: 16 bytes
     *    type: 4 bytes
     *    id: 4 bytes
     *    length: 4 bytes
     *    padding: 4 bytes
     */
    std::shared_ptr<WebXInstruction> createWindowsInstruction(uint32_t instructionId, WebXBinaryBuffer & buffer) const;

    /**
     * Create a quality instruction
     * Structure:
     *   Header: 16 bytes
     *    type: 4 bytes
     *    id: 4 bytes
     *    length: 4 bytes
     *    padding: 4 bytes
     *   Content: 4 bytes
     *    qualityIndex: 4 bytes
     */
    std::shared_ptr<WebXInstruction> createQualityInstruction(uint32_t instructionId, WebXBinaryBuffer & buffer) const;

private:

};


#endif //WEBX_INSTRUCTION_DECODER_H
