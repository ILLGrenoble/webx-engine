#ifndef WEBX_MESSAGE_ENCODER_H
#define WEBX_MESSAGE_ENCODER_H

#include <memory>
#include <cstring>
#include <array>

namespace zmq {
class message_t;
}

/*
 * WebXMessageEncoder
 * This class is responsible for encoding various types of WebX messages
 * into binary data encapsulated by ZeroMQ message_t objects for transmission.
 */
class WebXMessage;
class WebXCursorImageMessage;
class WebXImageMessage;
class WebXMouseMessage;
class WebXScreenMessage;
class WebXSubImagesMessage;
class WebXMessageEncoder;
class WebXWindowsMessage;
class WebXPingMessage;
class WebXDisconnectMessage;
class WebXQualityMessage;
class WebXClipboardMessage;

class WebXMessageEncoder {
    public:
    /*
     * Constructor
     * Initializes the encoder with a session ID.
     * @param sessionId: A 16-byte array representing the session ID.
     */
    WebXMessageEncoder(const std::array<unsigned char, 16> & sessionId) {
        memcpy(this->_sessionId, sessionId.data(), 16);
    }

    /*
     * Destructor
     * Cleans up resources used by the encoder.
     */
    ~WebXMessageEncoder() {}

    /*
     * Encodes a WebXMessage into binary data encapsulated by a ZeroMQ message.
     * @param message: A shared pointer to the WebXMessage to encode.
     * @return A pointer to the encoded ZeroMQ message.
     */
    zmq::message_t * encode(std::shared_ptr<WebXMessage> message) const;

private:

    /*
     * Structure:
     * Header: 48 bytes
     *   sessionId: 16 bytes
     *   clientIndexMask: 8 bytes
     *   timestampMs: 8 bytes
     *   type: 4 bytes
     *   id: 4 bytes
     *   length: 4 bytes
     *   padding: 4 bytes
     * Content:
     *   commandId: 4 bytes
     *   x: 4 bytes
     *   y: 4 bytes
     *   xHot: 4 bytes
     *   yHot: 4 bytes
     *   id: 4 bytes
     *   imageDataLength: 4 bytes
     *   imageData: n bytes
     */
    zmq::message_t * createCursorImageMessage(std::shared_ptr<WebXCursorImageMessage> message) const;

    /*
     * Structure:
     * Header: 48 bytes
     *   sessionId: 16 bytes
     *   clientIndexMask: 8 bytes
     *   timestampMs: 8 bytes
     *   type: 4 bytes
     *   id: 4 bytes
     *   length: 4 bytes
     *   padding: 4 bytes
     * Content:
     *   commandId: 4 bytes
     *   windowId: 4 bytes
     *   depth: 4 bytes
     *   imageType: 4 bytes (chars)
     *   imageDataLength: 4 bytes
     *   alphaDataLength: 4 bytes (0 if doesn't exit)
     *   imageData: n bytes
     *   alphaData: n bytes (optional)
     */
    zmq::message_t * createImageMessage(std::shared_ptr<WebXImageMessage> message) const;

    /*
     * Structure:
     * Header: 48 bytes
     *   sessionId: 16 bytes
     *   clientIndexMask: 8 bytes
     *   timestampMs: 8 bytes
     *   type: 4 bytes
     *   id: 4 bytes
     *   length: 4 bytes
     *   padding: 4 bytes
     * Content:
     *   commandId: 4 bytes
     *   x: 4 bytes
     *   y: 4 bytes
     *   cursorId: 4 bytes
     */
    zmq::message_t * createMouseMessage(std::shared_ptr<WebXMouseMessage> message) const;

    /*
     * Structure:
     * Header: 48 bytes
     *   sessionId: 16 bytes
     *   clientIndexMask: 8 bytes
     *   timestampMs: 8 bytes
     *   type: 4 bytes
     *   id: 4 bytes
     *   length: 4 bytes
     *   padding: 4 bytes
     * Content:
     *   commandId: 4 bytes
     *   screenWidth: 4 bytes
     *   screenHeight: 4 bytes
     */
    zmq::message_t * createScreenMessage(std::shared_ptr<WebXScreenMessage> message) const;

    /*
     * Structure:
     * Header: 48 bytes
     *   sessionId: 16 bytes
     *   clientIndexMask: 8 bytes
     *   timestampMs: 8 bytes
     *   type: 4 bytes
     *   id: 4 bytes
     *   length: 4 bytes
     *   padding: 4 bytes
     * Content:
     *   commandId: 4 bytes
     *   windowId: 4 bytes
     *   # subimages: 4 bytes
     *   Subimages:
     *     x: 4 bytes
     *     y: 4 bytes
     *     width: 4 bytes
     *     height: 4 bytes
     *     depth: 4 bytes
     *     imageType: 4 bytes (chars)
     *     imageDataLength: 4 bytes
     *     alphaDataLength: 4 bytes (0 if no alpha data)
     *     imageData: n bytes
     *     alphaData: n bytes (optional)
     */
    zmq::message_t * createSubImagesMessage(std::shared_ptr<WebXSubImagesMessage> message) const;

    /*
     * Structure:
     * Header: 48 bytes
     *   sessionId: 16 bytes
     *   clientIndexMask: 8 bytes
     *   timestampMs: 8 bytes
     *   type: 4 bytes
     *   id: 4 bytes
     *   length: 4 bytes
     *   padding: 4 bytes
     * Content:
     *   commandId: 4 bytes
     *   # windows: 4 bytes
     *   Window: 20 bytes
     *      id: 4 bytes
     *      x: 4 bytes
     *      y: 4 bytes
     *      width: 4 bytes
     *      height: 4 bytes
     */
    zmq::message_t * createWindowsMessage(std::shared_ptr<WebXWindowsMessage> message) const;

    /*
     * Structure:
     * Header: 48 bytes
     *   sessionId: 16 bytes
     *   clientIndexMask: 8 bytes
     *   timestampMs: 8 bytes
     *   type: 4 bytes
     *   id: 4 bytes
     *   length: 4 bytes
     *   padding: 4 bytes
     * Content:
     */
    zmq::message_t * createPingMessage(std::shared_ptr<WebXPingMessage> message) const;

    /*
     * Structure:
     * Header: 48 bytes
     *   sessionId: 16 bytes
     *   clientIndexMask: 8 bytes
     *   timestampMs: 8 bytes
     *   type: 4 bytes
     *   id: 4 bytes
     *   length: 4 bytes
     *   padding: 4 bytes
     * Content:
     */
    zmq::message_t * createDisconnectMessage(std::shared_ptr<WebXDisconnectMessage> message) const;

    /*
     * Structure:
     * Header: 48 bytes
     *   sessionId: 16 bytes
     *   clientIndexMask: 8 bytes
     *   timestampMs: 8 bytes
     *   type: 4 bytes
     *   id: 4 bytes
     *   length: 4 bytes
     *   padding: 4 bytes
     * Content:
     *   index: 4 bytes
     *   imageFPS: 4 bytes
     *   rgbQuality: 4 bytes
     *   alphaQuality: 4 bytes
     *   maxMbps: 4 bytes
     */
    zmq::message_t * createQualityMessage(std::shared_ptr<WebXQualityMessage> message) const;

    /*
     * Structure:
     * Header: 48 bytes
     *   sessionId: 16 bytes
     *   clientIndexMask: 8 bytes
     *   timestampMs: 8 bytes
     *   type: 4 bytes
     *   id: 4 bytes
     *   length: 4 bytes
     *   padding: 4 bytes
     * Content:
     *   clipboardContentLength: 4 bytes
     *   clipboardContent: n bytes
     */
    zmq::message_t * createClipboardMessage(std::shared_ptr<WebXClipboardMessage> message) const;

private:
    const static int MESSAGE_HEADER_LENGTH = 48;
    unsigned char _sessionId[16];
};


#endif //WEBX_MESSAGE_ENCODER_H
