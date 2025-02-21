
#ifndef WEBX_MESSAGE_ENCODER_H
#define WEBX_MESSAGE_ENCODER_H

#include <memory>
#include <cstring>

namespace zmq {
class message_t;
}

class WebXMessage;
class WebXCursorImageMessage;
class WebXImageMessage;
class WebXMouseMessage;
class WebXScreenMessage;
class WebXSubImagesMessage;
class WebXMessageEncoder;
class WebXWindowsMessage;

class WebXMessageEncoder {
    public:
    WebXMessageEncoder(const unsigned char * sessionId) {
        memcpy(this->_sessionId, sessionId, 16);
    }
    ~WebXMessageEncoder() {}

    zmq::message_t * encode(std::shared_ptr<WebXMessage> message);

private:

    // Structure:
    // Header: 32 bytes
    //   sessionId: 16 bytes
    //   type: 4 bytes
    //   id: 4 bytes
    //   length: 4 bytes
    //   padding: 4 bytes
    // Content:
    //   commandId: 4 bytes
    //   x: 4 bytes
    //   y: 4 bytes
    //   xHot: 4 bytes
    //   yHot: 4 bytes
    //   id: 4 bytes
    //   imageDataLength: 4 bytes
    //   imageData: n bytes
    zmq::message_t * createCursorImageMessage(std::shared_ptr<WebXCursorImageMessage> message);

    // Structure:
    // Header: 32 bytes
    //   sessionId: 16 bytes
    //   type: 4 bytes
    //   id: 4 bytes
    //   length: 4 bytes
    //   padding: 4 bytes
    // Content:
    //   commandId: 4 bytes
    //   windowId: 4 bytes
    //   depth: 4 bytes
    //   imageType: 4 bytes (chars)
    //   imageDataLength: 4 bytes
    //   alphaDataLength: 4 bytes (0 if doesn't exit)
    //   imageData: n bytes
    //   alphaData: n bytes (optional)
    zmq::message_t * createImageMessage(std::shared_ptr<WebXImageMessage> message);


    // Structure:
    // Header: 32 bytes
    //   sessionId: 16 bytes
    //   type: 4 bytes
    //   id: 4 bytes
    //   length: 4 bytes
    //   padding: 4 bytes
    // Content:
    //   commandId: 4 bytes
    //   x: 4 bytes
    //   y: 4 bytes
    //   cursorId: 4 bytes
    zmq::message_t * createMouseMessage(std::shared_ptr<WebXMouseMessage> message);

    // Structure:
    // Header: 32 bytes
    //   sessionId: 16 bytes
    //   type: 4 bytes
    //   id: 4 bytes
    //   length: 4 bytes
    //   padding: 4 bytes
    // Content:
    //   commandId: 4 bytes
    //   screenWidth: 4 bytes
    //   screenHeight: 4 bytes
    zmq::message_t * createScreenMessage(std::shared_ptr<WebXScreenMessage> message);

    // Structure:
    // Header: 32 bytes
    //   sessionId: 16 bytes
    //   type: 4 bytes
    //   id: 4 bytes
    //   length: 4 bytes
    //   padding: 4 bytes
    // Content:
    //   commandId: 4 bytes
    //   windowId: 4 bytes
    //   # subimages: 4 bytes
    //   Subimages:
    //     x: 4 bytes
    //     y: 4 bytes
    //     width: 4 bytes
    //     height: 4 bytes
    //     depth: 4 bytes
    //     imageType: 4 bytes (chars)
    //     imageDataLength: 4 bytes
    //     alphaDataLength: 4 bytes (0 if no alpha data)
    //     imageData: n bytes
    //     alphaData: n bytes (optional)
    zmq::message_t * createSubImagesMessage(std::shared_ptr<WebXSubImagesMessage> message);

    // Structure:
    // Header: 32 bytes
    //   sessionId: 16 bytes
    //   type: 4 bytes
    //   id: 4 bytes
    //   length: 4 bytes
    //   padding: 4 bytes
    // Content:
    //   commandId: 4 bytes
    //   # windows: 4 bytes
    //   Window: 20 bytes
    //      id: 4 bytes
    //      x: 4 bytes
    //      y: 4 bytes
    //      width: 4 bytes
    //      height: 4 bytes
    zmq::message_t * createWindowsMessage(std::shared_ptr<WebXWindowsMessage> message);

private:
    unsigned char _sessionId[16];
};


#endif //WEBX_MESSAGE_ENCODER_H
