#ifndef WEBX_CLIENT_MESSAGE_PUBLISHER_H
#define WEBX_CLIENT_MESSAGE_PUBLISHER_H

#include <display/WebXConnection.h>
#include <utils/WebXQueue.h>
#include <thread>
#include <mutex>

class WebXMessage;
class WebXBinarySerializer;

namespace zmq {
class context_t;
}

class WebXClientMessagePublisher : public WebXConnection {
public:
    WebXClientMessagePublisher();
    virtual ~WebXClientMessagePublisher();

    void run(WebXBinarySerializer * serializer, zmq::context_t * context, int port);
    void stop();

public:
    virtual void onDisplayChanged(const std::vector<WebXWindowProperties> & windows);
    virtual void onImageChanged(unsigned long windowId, std::shared_ptr<WebXImage> image);
    virtual void onSubImagesChanged(unsigned long windowId, const std::vector<WebXSubImage> & subImages);
    virtual void onMouseChanged(int x, int y, uint32_t cursorId);

private:
    void mainLoop();

private:
    std::thread * _thread;
    std::mutex _mutex;
    bool _running;
    WebXQueue<std::shared_ptr<WebXMessage>> _messageQueue;

    WebXBinarySerializer * _serializer;
    zmq::context_t * _context;
    int _port;
};


#endif /* WEBX_CLIENT_MESSAGE_PUBLISHER_H */