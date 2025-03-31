#ifndef WEBX_CLIENT_CONNECTOR_H
#define WEBX_CLIENT_CONNECTOR_H

#include <models/WebXSettings.h>
#include <gateway/WebXGateway.h>
#include "WebXZMQ.h"
#include <thread>
#include <mutex>

namespace zmq {
class context_t;
class socket_t;
}

/**
 * @class WebXClientConnector
 * @brief Manages the connection between a client and the WebX system.
 * 
 * This class handles client connections, disconnections, and message exchanges
 * using ZeroMQ sockets. It also manages the state of the connection.
 */
class WebXClientConnector {
private:
    /**
     * @enum SocketState
     * @brief Represents the state of the socket.
     */
    enum SocketState {
        WAITING_RECV = 0,
        WAITING_SEND,
    };

public:
    /**
     * @brief Constructs a WebXClientConnector with the given settings and gateway.
     * @param settings The transport settings to configure the connector.
     * @param gateway The gateway to interact with the WebX system.
     */
    WebXClientConnector(const WebXTransportSettings & settings, WebXGateway & gateway);

    /**
     * @brief Destructor to clean up resources.
     */
    virtual ~WebXClientConnector();

    /**
     * @brief Starts the client connector.
     * @param context The ZeroMQ context to use.
     * @param clientAddr The address of the client to connect to.
     * @param eventBusAddr The address of the event bus.
     */
    void run(zmq::context_t * context, const std::string & clientAddr, const std::string & eventBusAddr);

    /**
     * @brief Stops the client connector.
     */
    void stop();

private:
    /**
     * @brief The main loop that manages client connections and message exchanges.
     */
    void mainLoop();

    /**
     * @brief Connects a client using the given session ID.
     * @param sessionId The session ID of the client.
     * @return The client ID string.
     */
    std::string connectClient(const std::string & sessionId);

    /**
     * @brief Disconnects a client using the given session ID and client ID.
     * @param sessionId The session ID of the client.
     * @param clientIdString The client ID string.
     * @return A status message indicating the result of the disconnection.
     */
    std::string disconnectClient(const std::string & sessionId, const std::string & clientIdString);

    /**
     * @brief Creates a ZeroMQ socket for responding to client requests.
     * @return The created ZeroMQ socket.
     */
    zmq::socket_t createClientResponder();

    /**
     * @brief Creates a ZeroMQ socket for subscribing to the event bus.
     * @return The created ZeroMQ socket.
     */
    zmq::socket_t createEventBusSubscriber();

    /**
     * @brief Sends a message to the client using the given socket.
     * @param clientResponder The ZeroMQ socket to use for sending the message.
     * @param messageText The message text to send.
     */
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