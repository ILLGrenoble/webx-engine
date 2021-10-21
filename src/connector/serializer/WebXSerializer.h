#ifndef WEBX_SERIALIZER_H
#define WEBX_SERIALIZER_H

#include <memory>
#include <connector/message/WebXMessage.h>

class WebXInstruction;

namespace zmq {
class message_t;
}

class WebXSerializer {
public:

    virtual std::shared_ptr<WebXInstruction> deserialize(const unsigned char * instructionData, size_t instructionDataSize) = 0;
    virtual zmq::message_t * serialize(std::shared_ptr<WebXMessage> message) = 0;
};

#endif /* WEBX_SERIALIZER_H */