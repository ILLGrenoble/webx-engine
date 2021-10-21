#ifndef WEBX_BINARY_SERIALIZER_H
#define WEBX_BINARY_SERIALIZER_H

#include "WebXSerializer.h"
#include "WebXInstructionDecoder.h"
#include <utils/WebXObjectPool.h>
#include <connector/instruction/WebXMouseInstruction.h>

class WebXScreenMessageBinarySerializer;
class WebXWindowsMessageBinarySerializer;
class WebXSubImagesMessageBinarySerializer;
class WebXImageMessageBinarySerializer;
class WebXConnectionMessageBinarySerializer;
class WebXMouseMessageBinarySerializer;
class WebXCursorImageMessageBinarySerializer;
class WebXVoidMessageBinarySerializer;

class WebXBinarySerializer : public WebXSerializer {
public:
    WebXBinarySerializer();
    virtual ~WebXBinarySerializer();

    virtual std::shared_ptr<WebXInstruction> deserialize(const unsigned char * instructionData, size_t instructionDataSize);
    virtual zmq::message_t * serialize(std::shared_ptr<WebXMessage> message);

private:
    WebXObjectPool<WebXMouseInstruction> _webXMouseInstructionPool;

    WebXScreenMessageBinarySerializer * _screenMessageSerializer;
    WebXWindowsMessageBinarySerializer * _windowsMessageSerializer;
    WebXSubImagesMessageBinarySerializer * _subImagesMessageSerializer;
    WebXImageMessageBinarySerializer * _imageMessageSerializer;
    WebXConnectionMessageBinarySerializer * _connectionMessageSerializer;
    WebXMouseMessageBinarySerializer * _mouseMessageSerializer;
    WebXCursorImageMessageBinarySerializer * _cursorImageMessageSerializer;
    WebXVoidMessageBinarySerializer * _voidMessageSerializer;
    WebXInstructionDecoder * _instructionDecoder;
};


#endif /* WEBX_BINARY_SERIALIZER_H */