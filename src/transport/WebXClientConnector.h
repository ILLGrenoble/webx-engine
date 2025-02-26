#ifndef WEBX_CLIENT_CONNECTOR_H
#define WEBX_CLIENT_CONNECTOR_H

#include <message/WebXMessage.h>
#include <utils/WebXSettings.h>
#include <gateway/WebXGateway.h>
#include "WebXZMQ.h"
#include <thread>
#include <mutex>


namespace zmq {
class context_t;
class socket_t;
}

class WebXClientConnector {
public:
    WebXClientConnector(const WebXTransportSettings & settings, WebXGateway & gateway);
    virtual ~WebXClientConnector();

    void run(zmq::context_t * context, const std::string & clientAddr, const std::string & eventBusAddr);
    void stop();

private:
    void mainLoop();
    zmq::socket_t createClientResponder();
    zmq::socket_t createEventBusSubscriber();
    void sendMessage(zmq::socket_t & clientResponder, zmq::message_t & message);

private:
    std::thread * _thread;

    WebXGateway & _gateway;

    int _publisherPort;
    int _collectorPort;
    std::string _sessionId;

    zmq::context_t * _context;
    std::string _clientAddr;
    std::string _eventBusAddr;
};


#endif /* WEBX_CLIENT_CONNECTOR_H */