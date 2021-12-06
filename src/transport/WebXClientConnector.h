#ifndef WEBX_CLIENT_CONNECTOR_H
#define WEBX_CLIENT_CONNECTOR_H

#include "message/WebXMessage.h"
#include "WebXZMQ.h"
#include <thread>
#include <mutex>


class WebXBinarySerializer;
namespace zmq {
class context_t;
class socket_t;
}

class WebXClientConnector {
public:
    WebXClientConnector();
    virtual ~WebXClientConnector();
    void init();

    void run(WebXBinarySerializer * serializer, zmq::context_t * context, const std::string & clientAddr, const std::string & eventBusAddr, int publisherPort, int collectorPort);
    void stop();

private:
    void mainLoop();
    zmq::socket_t createClientResponder();
    zmq::socket_t createEventBusSubscriber();

private:
    std::thread * _thread;

    WebXBinarySerializer * _serializer;
    zmq::context_t * _context;
    std::string _clientAddr;
    std::string _eventBusAddr;
    int _publisherPort;
    int _collectorPort;
};


#endif /* WEBX_CLIENT_CONNECTOR_H */