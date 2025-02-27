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
private:
    enum SocketState {
        WAITING_RECV = 0,
        WAITING_SEND,
    };

public:
    WebXClientConnector(const WebXTransportSettings & settings, WebXGateway & gateway);
    virtual ~WebXClientConnector();

    void run(zmq::context_t * context, const std::string & clientAddr, const std::string & eventBusAddr);
    void stop();

private:
    void mainLoop();

    std::string connectClient(const std::string & sessionId);
    std::string disconnectClient(const std::string & sessionId, const std::string & clientIdString);

    zmq::socket_t createClientResponder();
    zmq::socket_t createEventBusSubscriber();
    void sendMessage(zmq::socket_t & clientResponder, const std::string & messageText);

private:
    std::thread * _thread;

    WebXGateway & _gateway;

    int _publisherPort;
    int _collectorPort;
    std::string _sessionId;

    zmq::context_t * _context;
    std::string _clientAddr;
    std::string _eventBusAddr;
    SocketState _socketState;
};


#endif /* WEBX_CLIENT_CONNECTOR_H */