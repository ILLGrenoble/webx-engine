#ifndef WEBX_BINARY_SERIALIZER_H
#define WEBX_BINARY_SERIALIZER_H

#include "WebXSerializer.h"
#include <nlohmann/json.hpp>

class WebXBinarySerializer : public WebXSerializer {
public:
    WebXBinarySerializer() :
        WebXSerializer("json") {}
    virtual ~WebXBinarySerializer() {}

    virtual WebXInstruction deserialize(void * instructionData, size_t instructionDataSize);
    virtual zmq::message_t * serialize(WebXMessage * message);

};

void from_json(const nlohmann::json& j, WebXInstruction & request);

#endif /* WEBX_BINARY_SERIALIZER_H */