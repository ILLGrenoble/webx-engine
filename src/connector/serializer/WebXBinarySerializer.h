#ifndef WEBX_BINARY_SERIALIZER_H
#define WEBX_BINARY_SERIALIZER_H

#include "WebXSerializer.h"
#include <nlohmann/json.hpp>
#include <utils/WebXObjectPool.h>
#include <connector/instruction/WebXMouseInstruction.h>

class WebXScreenMessageBinarySerializer;
class WebXWindowsMessageBinarySerializer;
class WebXSubImagesMessageBinarySerializer;
class WebXImageMessageBinarySerializer;
class WebXConnectionMessageBinarySerializer;
class WebXMouseMessageBinarySerializer;
class WebXCursorImageMessageBinarySerializer;

class WebXBinarySerializer : public WebXSerializer {
public:
    WebXBinarySerializer();
    virtual ~WebXBinarySerializer();

    virtual std::shared_ptr<WebXInstruction> deserialize(void * instructionData, size_t instructionDataSize);
    virtual zmq::message_t * serialize(WebXMessage * message);

private:
    WebXObjectPool<WebXMouseInstruction> _webXMouseInstructionPool;

    WebXScreenMessageBinarySerializer * _screenMessageSerializer;
    WebXWindowsMessageBinarySerializer * _windowsMessageSerializer;
    WebXSubImagesMessageBinarySerializer * _subImagesMessageSerializer;
    WebXImageMessageBinarySerializer * _imageMessageSerializer;
    WebXConnectionMessageBinarySerializer * _connectionMessageSerializer;
    WebXMouseMessageBinarySerializer * _mouseMessageSerializer;
    WebXCursorImageMessageBinarySerializer * _cursorImageMessageSerializer;
};

void from_json(const nlohmann::json& j, WebXInstruction & request);

#endif /* WEBX_BINARY_SERIALIZER_H */