#include "WebXBinarySerializer.h"
#include "WebXInstructionDecoder.h"
#include "WebXMessageEncoder.h"

WebXBinarySerializer::WebXBinarySerializer(WebXSettings * settings) :
     _messageEncoder(new WebXMessageEncoder(settings)),
     _instructionDecoder(new WebXInstructionDecoder()) {
}

WebXBinarySerializer::~WebXBinarySerializer() {
    delete _messageEncoder;
    delete _instructionDecoder;
}

std::shared_ptr<WebXInstruction> WebXBinarySerializer::deserialize(const unsigned char * instructionData, size_t instructionDataSize) {
    return this->_instructionDecoder->decode(instructionData, instructionDataSize);
}

zmq::message_t * WebXBinarySerializer::serialize(std::shared_ptr<WebXMessage> message) {
    return this->_messageEncoder->encode(message);    
}