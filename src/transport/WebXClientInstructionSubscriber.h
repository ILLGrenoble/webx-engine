#ifndef WEBX_CLIENT_INSTRUCTION_SUBSCRIBER_H
#define WEBX_CLIENT_INSTRUCTION_SUBSCRIBER_H

#include "serializer/WebXInstructionDecoder.h"
#include <gateway/WebXGateway.h>
#include <models/WebXSettings.h>
#include <thread>
#include <mutex>
#include <string>

class WebXGateway;
class WebXInstruction;

namespace zmq {
class context_t;
class socket_t;
}

/**
 * @class WebXClientInstructionSubscriber
 * @brief Subscribes to instructions from a client and processes them.
 * 
 * This class manages the lifecycle of a subscriber that listens for instructions
 * from a client and forwards them to the WebXGateway for processing.
 */
class WebXClientInstructionSubscriber {
public:
    /**
     * @brief Constructs a WebXClientInstructionSubscriber with the given settings and gateway.
     * @param settings The transport settings to configure the subscriber.
     * @param gateway The gateway to forward instructions to.
     */
    WebXClientInstructionSubscriber(const WebXTransportSettings & settings, WebXGateway & gateway);

    /**
     * @brief Destructor to clean up resources.
     */
    virtual ~WebXClientInstructionSubscriber();

    /**
     * @brief Starts the instruction subscriber.
     * @param context The ZeroMQ context to use.
     * @param clientAddr The address of the client to subscribe to.
     * @param bindToClientAddr Whether to bind to the client address or connect to it.
     */
    void run(zmq::context_t * context, const std::string & clientAddr, bool bindToClientAddr);

    /**
     * @brief Stops the instruction subscriber.
     */
    void stop();

private:
    /**
     * @brief The main loop that listens for and processes instructions.
     */
    void mainLoop();

    /**
     * @brief Creates a ZeroMQ socket for subscribing to client instructions.
     * @return The created ZeroMQ socket.
     */
    zmq::socket_t createClientInstructionSubscriber();

    /**
     * @brief Creates a ZeroMQ socket for subscribing to the event bus.
     * @return The created ZeroMQ socket.
     */
    zmq::socket_t createEventBusSubscriber();

private:
    std::thread * _thread;

    WebXGateway & _gateway;
    const WebXInstructionDecoder _decoder;
    zmq::context_t * _context;
    std::string _clientAddr;
    bool _bindToClientAddr;
    std::string _eventBusAddr;
    unsigned char _sessionId[16];
};

#endif /* WEBX_CLIENT_INSTRUCTION_SUBSCRIBER_H */
