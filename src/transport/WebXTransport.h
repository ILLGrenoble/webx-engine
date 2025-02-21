#ifndef WEBX_TRANSPORT_H
#define WEBX_TRANSPORT_H

#include "WebXZMQ.h"
#include <gateway/WebXGateway.h>
#include <utils/WebXSettings.h>

class WebXGateway;
class WebXBinarySerializer;
class WebXClientConnector;
class WebXClientMessagePublisher;
class WebXClientCommandCollector;

class WebXTransport {
public:
    WebXTransport(WebXGateway & gateway, const WebXSettings & settings, bool standAlone);
    virtual ~WebXTransport();

    void start();
    void stop();

private:
    zmq::socket_t createEventBusPublisher();

private:
    const WebXSettings & _settings;
    WebXGateway & _gateway;
    bool _standAlone;

    WebXBinarySerializer * _serializer;

    zmq::context_t _context;
    zmq::socket_t _eventBusPublisher;

    WebXClientConnector * _connector;
    WebXClientMessagePublisher * _publisher;
    WebXClientCommandCollector * _collector;
};


#endif /* WEBX_TRANSPORT_H */
