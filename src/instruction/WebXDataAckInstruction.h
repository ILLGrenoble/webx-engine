#ifndef WEBX_DATA_ACK_INSTRUCTION_H
#define WEBX_DATA_ACK_INSTRUCTION_H

#include <chrono>
#include "WebXInstruction.h"

class WebXDataAckInstruction : public WebXInstruction {

public:
    WebXDataAckInstruction(uint32_t clientId, uint32_t id, uint64_t sendTimestampMs, uint32_t dataLength) :
        WebXInstruction(Type::DataAck, clientId, id),
        sendTimestampMs(sendTimestampMs),
        recvTimestampMs(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()),
        dataLength(dataLength) {}
    virtual ~WebXDataAckInstruction() {}

    uint64_t sendTimestampMs;
    uint64_t recvTimestampMs;
    uint32_t dataLength;
};


#endif //WEBX_DATA_ACK_INSTRUCTION_H
