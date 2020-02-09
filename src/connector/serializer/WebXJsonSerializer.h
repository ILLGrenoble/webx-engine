#ifndef WEBX_JSON_SERIALIZER_H
#define WEBX_JSON_SERIALIZER_H

#include "WebXSerializer.h"
#include <nlohmann/json.hpp>
#include <utils/WebXObjectPool.h>
#include <connector/instruction/WebXMouseInstruction.h>

class WebXJsonSerializer : public WebXSerializer {
public:
    WebXJsonSerializer() :
        WebXSerializer("json") {}
    virtual ~WebXJsonSerializer() {}

    virtual std::shared_ptr<WebXInstruction> deserialize(void * instructionData, size_t instructionDataSize);
    virtual zmq::message_t * serialize(WebXMessage * message);

private:
    WebXObjectPool<WebXMouseInstruction> _webXMouseInstructionPool;

};


#endif /* WEBX_JSON_SERIALIZER_H */