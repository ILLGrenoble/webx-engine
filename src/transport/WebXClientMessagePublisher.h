#ifndef WEBX_CLIENT_MESSAGE_PUBLISHER_H
#define WEBX_CLIENT_MESSAGE_PUBLISHER_H

#include "serializer/WebXMessageEncoder.h"
#include <utils/WebXQueue.h>
#include <models/WebXSettings.h>
#include <thread>
#include <mutex>

class WebXMessage;

namespace zmq {
class context_t;
class socket_t;
}

/**
 * @class WebXClientMessagePublisher
 * @brief Publishes messages to a client using ZeroMQ.
 * 
 * This class is responsible for managing the lifecycle of a message publisher
 * that sends messages to a client. It uses a queue to handle incoming messages
 * and a separate thread to process and publish them.
 */
class WebXClientMessagePublisher {
public:
    /**
     * @brief Constructs a WebXClientMessagePublisher with the given settings.
     * @param settings The transport settings to configure the publisher.
     */
    WebXClientMessagePublisher(const WebXTransportSettings & settings);

    /**
     * @brief Destructor to clean up resources.
     */
    virtual ~WebXClientMessagePublisher();

    /**
     * @brief Starts the message publisher.
     * @param context The ZeroMQ context to use.
     * @param clientAddr The address of the client to publish messages to.
     * @param bindToClientAddr Whether to bind to the client address or connect to it.
     */
    void run(zmq::context_t * context, const std::string & clientAddr, bool bindToClientAddr);

    /**
     * @brief Stops the message publisher.
     */
    void stop();

    /**
     * @brief Adds a message to the queue for publishing.
     * @param message The message to be published.
     */
    void onMessage(std::shared_ptr<WebXMessage> message) {
        this->_messageQueue.put(message);
    }

private:
    /**
     * @brief The main loop that processes and publishes messages.
     */
    void mainLoop();

    /**
     * @brief Creates a ZeroMQ socket for publishing messages to the client.
     * @return The created ZeroMQ socket.
     */
    zmq::socket_t createClientMessagePublisher();

private:
    std::thread * _thread;
    bool _running;
    WebXQueue<std::shared_ptr<WebXMessage>> _messageQueue;

    const WebXMessageEncoder _encoder;
    zmq::context_t * _context;
    std::string _eventBusAddr;
    std::string _clientAddr;
    bool _bindToClientAddr;
};

#endif /* WEBX_CLIENT_MESSAGE_PUBLISHER_H */