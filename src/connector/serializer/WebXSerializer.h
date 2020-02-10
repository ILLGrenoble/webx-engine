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
    WebXSerializer(const std::string & type) :
        _type(type) {}
    virtual ~WebXSerializer() {}

    const std::string getType() const {
        return this->_type;
    }

    virtual std::shared_ptr<WebXInstruction> deserialize(void * instructionData, size_t instructionDataSize) = 0;
    virtual zmq::message_t * serialize(std::shared_ptr<WebXMessage> message) = 0;

private:
    std::string _type;
};


#endif /* WEBX_SERIALIZER_H */