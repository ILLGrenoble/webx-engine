#ifndef WEBX_TRANSPORT_H
#define WEBX_TRANSPORT_H

#include "WebXZMQ.h"
#include <gateway/WebXGateway.h>
#include <models/WebXSettings.h>

class WebXGateway;
class WebXBinarySerializer;
class WebXClientConnector;
class WebXClientMessagePublisher;
class WebXClientInstructionSubscriber;

/**
 * @brief Manages the transport layer of the WebX Engine, including client connections, message publishing, and instruction handling.
 */
class WebXTransport {
public:
    /**
     * @brief Constructs a WebXTransport instance.
     * @param gateway Reference to the WebXGateway.
     * @param settings Transport settings for the WebX Engine.
     * @param standAlone Indicates whether the engine is running in standalone mode.
     */
    WebXTransport(WebXGateway & gateway, const WebXTransportSettings & settings, bool standAlone);

    /**
     * @brief Destructor to clean up resources.
     */
    virtual ~WebXTransport();

    /**
     * @brief Starts the transport layer, initializing connectors, publishers, and subscribers.
     */
    void start();

    /**
     * @brief Stops the transport layer, shutting down all components and cleaning up resources.
     */
    void stop();

private:
    /**
     * @brief Creates a ZeroMQ publisher socket for the internal event bus.
     * @return A configured ZeroMQ publisher socket.
     */
    zmq::socket_t createEventBusPublisher();

private:
    const WebXTransportSettings & _settings;
    WebXGateway & _gateway;
    bool _standAlone;

    zmq::context_t _context;
    zmq::socket_t _eventBusPublisher;

    WebXClientConnector * _connector;
    WebXClientMessagePublisher * _publisher;
    WebXClientInstructionSubscriber * _collector;
};

#endif /* WEBX_TRANSPORT_H */
