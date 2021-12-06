#ifndef WEBX_CLIENT_MESSAGE_PUBLISHER_H
#define WEBX_CLIENT_MESSAGE_PUBLISHER_H

#include <controller/WebXConnection.h>
#include <utils/WebXQueue.h>
#include <thread>
#include <mutex>

class WebXMessage;
class WebXBinarySerializer;

namespace zmq {
class context_t;
class socket_t;
}

class WebXClientMessagePublisher : public WebXConnection {
public:
    WebXClientMessagePublisher();
    virtual ~WebXClientMessagePublisher();

    void run(WebXBinarySerializer * serializer, zmq::context_t * context, const std::string & clientAddr, bool bindToClientAddr, const std::string & eventBusAddr);
    void stop();

public:
    virtual void onMessage(std::shared_ptr<WebXMessage> message) {
        this->_messageQueue.put(message);
    }

private:
    void mainLoop();
    zmq::socket_t createClientMessagePublisher();


private:
    std::thread * _thread;
    bool _running;
    WebXQueue<std::shared_ptr<WebXMessage>> _messageQueue;

    WebXBinarySerializer * _serializer;
    zmq::context_t * _context;
    std::string _clientAddr;
    bool _bindToClientAddr;
    std::string _eventBusAddr;
};


#endif /* WEBX_CLIENT_MESSAGE_PUBLISHER_H */