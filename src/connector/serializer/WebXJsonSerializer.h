#ifndef WEBX_JSON_SERIALIZER_H
#define WEBX_JSON_SERIALIZER_H

#include "WebXSerializer.h"
#include <nlohmann/json.hpp>

class WebXJsonSerializer : public WebXSerializer {
public:
    WebXJsonSerializer() :
        WebXSerializer("json") {}
    virtual ~WebXJsonSerializer() {}

    virtual WebXInstruction * deserialize(void * instructionData, size_t instructionDataSize);
    virtual zmq::message_t * serialize(WebXMessage * message);

};


#endif /* WEBX_JSON_SERIALIZER_H */