#ifndef WEBX_BINARY_SERIALIZER_H
#define WEBX_BINARY_SERIALIZER_H

#include <memory>
#include "WebXInstructionDecoder.h"
#include "WebXMessageEncoder.h"

class WebXInstruction;
class WebXMessage;

namespace zmq {
class message_t;
}

class WebXBinarySerializer {
public:
    WebXBinarySerializer(const std::array<unsigned char, 16> & sessionId) :
        _messageEncoder(sessionId) {
    }
    ~WebXBinarySerializer() {
    }

    std::shared_ptr<WebXInstruction> deserialize(const unsigned char * instructionData, size_t instructionDataSize) const {
        return this->_instructionDecoder.decode(instructionData, instructionDataSize);
    }
    
    zmq::message_t * serialize(std::shared_ptr<WebXMessage> message) const {
        return this->_messageEncoder.encode(message);    
    }
private:
    const WebXMessageEncoder _messageEncoder;
    const WebXInstructionDecoder _instructionDecoder;
};


#endif /* WEBX_BINARY_SERIALIZER_H */