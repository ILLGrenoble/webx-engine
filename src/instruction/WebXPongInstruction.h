#ifndef WEBX_PONG_INSTRUCTION_H
#define WEBX_PONG_INSTRUCTION_H

#include <chrono>
#include "WebXInstruction.h"

class WebXPongInstruction : public WebXInstruction {

public:
    WebXPongInstruction(uint32_t clientId, uint32_t id, uint64_t sendTimestampMs) :
        WebXInstruction(Type::Pong, clientId, id),
        sendTimestampMs(sendTimestampMs),
        recvTimestampMs(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) {}
    virtual ~WebXPongInstruction() {}

    uint64_t sendTimestampMs;
    uint64_t recvTimestampMs;
};


#endif //WEBX_PONG_INSTRUCTION_H
