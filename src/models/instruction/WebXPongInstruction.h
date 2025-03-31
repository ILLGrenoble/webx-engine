#ifndef WEBX_PONG_INSTRUCTION_H
#define WEBX_PONG_INSTRUCTION_H

#include <chrono>
#include "WebXInstruction.h"

/**
 * @class WebXPongInstruction
 * @brief Represents a Pong instruction for latency measurement.
 * 
 * This class inherits from WebXInstruction and is used to handle
 * Pong instructions. It includes timestamps for send and receive
 * times to measure Round-Trip Time latency.
 */
class WebXPongInstruction : public WebXInstruction {

public:
    /**
     * @brief Constructor for WebXPongInstruction.
     * @param clientId The ID of the client sending the instruction.
     * @param id The unique ID of the instruction.
     * @param sendTimestampMs The timestamp when the origin ping message was sent.
     */
    WebXPongInstruction(uint32_t clientId, uint32_t id, uint64_t sendTimestampMs) :
        WebXInstruction(Type::Pong, clientId, id),
        sendTimestampMs(sendTimestampMs),
        recvTimestampMs(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) {}

    /**
     * @brief Destructor for WebXPongInstruction.
     */
    virtual ~WebXPongInstruction() {}

    uint64_t sendTimestampMs;
    uint64_t recvTimestampMs;
};

#endif //WEBX_PONG_INSTRUCTION_H
