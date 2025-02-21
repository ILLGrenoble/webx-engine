#ifndef WEBX_CLIENT_MESSAGE_PUBLISHER_H
#define WEBX_CLIENT_MESSAGE_PUBLISHER_H

#include <serializer/WebXBinarySerializer.h>
#include <utils/WebXQueue.h>
#include <utils/WebXSettings.h>
#include <thread>
#include <mutex>

class WebXMessage;

namespace zmq {
class context_t;
class socket_t;
}

class WebXClientMessagePublisher {
public:
    WebXClientMessagePublisher(const WebXTransportSettings & settings, const WebXBinarySerializer & serializer);
    virtual ~WebXClientMessagePublisher();

    void run(zmq::context_t * context, const std::string & clientAddr, bool bindToClientAddr);
    void stop();

    void onMessage(std::shared_ptr<WebXMessage> message) {
        this->_messageQueue.put(message);
    }

private:
    void mainLoop();
    zmq::socket_t createClientMessagePublisher();


private:
    std::thread * _thread;
    bool _running;
    WebXQueue<std::shared_ptr<WebXMessage>> _messageQueue;

    const WebXBinarySerializer & _serializer;
    zmq::context_t * _context;
    std::string _eventBusAddr;
    std::string _clientAddr;
    bool _bindToClientAddr;
};


#endif /* WEBX_CLIENT_MESSAGE_PUBLISHER_H */