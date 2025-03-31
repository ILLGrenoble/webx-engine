#ifndef WEBX_DATA_ACK_INSTRUCTION_H
#define WEBX_DATA_ACK_INSTRUCTION_H

#include <chrono>
#include "WebXInstruction.h"

/**
 * @class WebXDataAckInstruction
 * @brief Represents a data acknowledgment instruction in the WebX engine.
 * 
 * This class is derived from WebXInstruction and is used to handle data acknowledgment events.
 * It includes timestamps and data length information.
 */
class WebXDataAckInstruction : public WebXInstruction {

public:
    /**
     * @brief Constructor for WebXDataAckInstruction.
     * @param clientId The ID of the client sending the instruction.
     * @param id The unique ID of the instruction.
     * @param sendTimestampMs The timestamp when the data was sent originally from the engine (in milliseconds).
     * @param dataLength The length of the data being acknowledged.
     */
    WebXDataAckInstruction(uint32_t clientId, uint32_t id, uint64_t sendTimestampMs, uint32_t dataLength) :
        WebXInstruction(Type::DataAck, clientId, id),
        sendTimestampMs(sendTimestampMs),
        recvTimestampMs(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()),
        dataLength(dataLength) {}
    
    /**
     * @brief Destructor for WebXDataAckInstruction.
     */
    virtual ~WebXDataAckInstruction() {}

    const uint64_t sendTimestampMs;
    const uint64_t recvTimestampMs;
    const uint32_t dataLength;
};

#endif //WEBX_DATA_ACK_INSTRUCTION_H
