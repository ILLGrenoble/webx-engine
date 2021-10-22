#include "WebXBinarySerializer.h"

WebXBinarySerializer::WebXBinarySerializer() :
     _messageEncoder(new WebXMessageEncoder()),
     _instructionDecoder(new WebXInstructionDecoder()) {
}

WebXBinarySerializer::~WebXBinarySerializer() {

}

std::shared_ptr<WebXInstruction> WebXBinarySerializer::deserialize(const unsigned char * instructionData, size_t instructionDataSize) {
    return this->_instructionDecoder->decode(instructionData, instructionDataSize);
}

zmq::message_t * WebXBinarySerializer::serialize(std::shared_ptr<WebXMessage> message) {
    return this->_messageEncoder->encode(message);    
}