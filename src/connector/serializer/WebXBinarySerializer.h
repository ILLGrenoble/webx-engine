#ifndef WEBX_BINARY_SERIALIZER_H
#define WEBX_BINARY_SERIALIZER_H

#include "WebXInstructionDecoder.h"
#include "WebXMessageEncoder.h"


class WebXBinarySerializer {
public:
    WebXBinarySerializer();
    ~WebXBinarySerializer();

    std::shared_ptr<WebXInstruction> deserialize(const unsigned char * instructionData, size_t instructionDataSize);
    zmq::message_t * serialize(std::shared_ptr<WebXMessage> message);

private:
    WebXMessageEncoder * _messageEncoder;
    WebXInstructionDecoder * _instructionDecoder;
};


#endif /* WEBX_BINARY_SERIALIZER_H */