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
    virtual void onMessage(std::shared_ptr<WebXMessage> message) {
        this->_messageQueue.put(message);
    }

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