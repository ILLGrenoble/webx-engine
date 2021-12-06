#ifndef WEBX_TRANSPORT_H
#define WEBX_TRANSPORT_H

#include "WebXZMQ.h"

class WebXBinarySerializer;
class WebXClientConnector;
class WebXClientMessagePublisher;
class WebXClientCommandCollector;

class WebXTransport {
public:
    WebXTransport(bool standAlone);
    virtual ~WebXTransport();

    void start();
    void stop();

private:
    zmq::socket_t createEventBusPublisher();

private:
    static int CONNECTOR_PORT;
    static int COLLECTOR_PORT;
    static int PUBLISHER_PORT;

    bool _standAlone;

    static std::string EVENT_BUS_ADDR;

    WebXBinarySerializer * _serializer;

    zmq::context_t _context;
    zmq::socket_t _eventBusPublisher;

    WebXClientConnector * _connector;
    WebXClientMessagePublisher * _publisher;
    WebXClientCommandCollector * _collector;
};


#endif /* WEBX_TRANSPORT_H */
