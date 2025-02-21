#ifndef WEBX_CLIENT_CONNECTOR_H
#define WEBX_CLIENT_CONNECTOR_H

#include <message/WebXMessage.h>
#include <utils/WebXSettings.h>
#include "WebXZMQ.h"
#include <thread>
#include <mutex>


namespace zmq {
class context_t;
class socket_t;
}

class WebXClientConnector {
public:
    WebXClientConnector(const WebXTransportSettings & settings);
    virtual ~WebXClientConnector();

    void run(zmq::context_t * context, const std::string & clientAddr, const std::string & eventBusAddr);
    void stop();

private:
    void mainLoop();
    zmq::socket_t createClientResponder();
    zmq::socket_t createEventBusSubscriber();

private:
    std::thread * _thread;
    int _publisherPort;
    int _collectorPort;

    zmq::context_t * _context;
    std::string _clientAddr;
    std::string _eventBusAddr;
};


#endif /* WEBX_CLIENT_CONNECTOR_H */