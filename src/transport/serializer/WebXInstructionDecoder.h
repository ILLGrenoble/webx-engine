#ifndef WEBX_INSTRUCTION_DECODER_H
#define WEBX_INSTRUCTION_DECODER_H

#include <memory>

/*
 * WebXInstructionDecoder
 * This class is responsible for decoding binary data into WebX instructions.
 */
class WebXBinaryBuffer;
class WebXInstruction;

class WebXInstructionDecoder {
public:
    /*
     * Constructor
     * Initializes the decoder.
     */
    WebXInstructionDecoder();

    /*
     * Destructor
     * Cleans up resources used by the decoder.
     */
    ~WebXInstructionDecoder();

    /*
     * Decodes binary instruction data into a WebXInstruction object.
     * @param instructionData: Pointer to the binary instruction data.
     * @param instructionDataSize: Size of the binary instruction data.
     * @return A shared pointer to the decoded WebXInstruction.
     */
    std::shared_ptr<WebXInstruction> decode(const unsigned char * instructionData, size_t instructionDataSize) const;

private:
    /*
     * Create a new mouse instruction
     * Structure:
     *   Header: 32 bytes
     *    sessionId: 16 bytes
     *    clientId: 4 bytes
     *    type: 4 bytes
     *    id: 4 bytes
     *    padding: 4 bytes
     *   Content: 4 bytes
     *    x: 4 bytes
     *    y: 4 bytes
     *    buttonMask: 4 bytes
     */
    std::shared_ptr<WebXInstruction> createMouseInstruction(uint32_t clientId, uint32_t instructionId, WebXBinaryBuffer & buffer) const;

    /*
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
    std::shared_ptr<WebXInstruction> createCursorImageInstruction(uint32_t clientId, uint32_t instructionId, WebXBinaryBuffer & buffer) const;

    /*
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
    std::shared_ptr<WebXInstruction> createImageInstruction(uint32_t clientId, uint32_t instructionId, WebXBinaryBuffer & buffer) const;

    /*
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
    std::shared_ptr<WebXInstruction> createKeyboardInstruction(uint32_t clientId, uint32_t instructionId, WebXBinaryBuffer & buffer) const;

    /*
     * Create a screen instruction
     * Structure:
     *   Header: 16 bytes
     *    type: 4 bytes
     *    id: 4 bytes
     *    length: 4 bytes
     *    padding: 4 bytes
     */
    std::shared_ptr<WebXInstruction> createScreenInstruction(uint32_t clientId, uint32_t instructionId, WebXBinaryBuffer & buffer) const;

    /*
     * Create a windows instruction
     * Structure:
     *   Header: 16 bytes
     *    type: 4 bytes
     *    id: 4 bytes
     *    length: 4 bytes
     *    padding: 4 bytes
     */
    std::shared_ptr<WebXInstruction> createWindowsInstruction(uint32_t clientId, uint32_t instructionId, WebXBinaryBuffer & buffer) const;

    /*
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
    std::shared_ptr<WebXInstruction> createQualityInstruction(uint32_t clientId, uint32_t instructionId, WebXBinaryBuffer & buffer) const;

    /*
     * Create a pong instruction
     * Structure:
     *   Header: 16 bytes
     *    type: 4 bytes
     *    id: 4 bytes
     *    length: 4 bytes
     *    padding: 4 bytes
     *   Content: 8 bytes
     *     sendTimestampMs: 8 bytes
     */
    std::shared_ptr<WebXInstruction> createPongInstruction(uint32_t clientId, uint32_t instructionId, WebXBinaryBuffer & buffer) const;

    /*
     * Create a data ack instruction
     * Structure:
     *   Header: 16 bytes
     *    type: 4 bytes
     *    id: 4 bytes
     *    length: 4 bytes
     *    padding: 4 bytes
     *   Content: 8 bytes
     *     sendTimestampMs: 8 bytes
     *     dataLength: 4 bytes
     */
    std::shared_ptr<WebXInstruction> createDataAckInstruction(uint32_t clientId, uint32_t instructionId, WebXBinaryBuffer & buffer) const;

private:

};


#endif //WEBX_INSTRUCTION_DECODER_H
