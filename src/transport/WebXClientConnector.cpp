#include "WebXClientConnector.h"
#include <utils/WebXHexUtils.h>
#include <unistd.h>
#include <string>
#include <spdlog/spdlog.h>

WebXClientConnector::WebXClientConnector(const WebXTransportSettings & settings, WebXGateway & gateway) :
    _thread(NULL),
    _gateway(gateway),
    _publisherPort(settings.publisherPort),
    _collectorPort(settings.collectorPort),
    _sessionId(settings.sessionIdString) {
}

WebXClientConnector::~WebXClientConnector() {
    if (this->_thread != NULL) {
        delete this->_thread;
        this->_thread = NULL;
    }
}

void WebXClientConnector::run(zmq::context_t * context, const std::string & clientAddr, const std::string & eventBusAddr) {
    this->_context = context;
    this->_clientAddr = clientAddr;
    this->_eventBusAddr = eventBusAddr;
    if (this->_thread == NULL) {
        this->_thread = new std::thread(&WebXClientConnector::mainLoop, this);
    }
}

void WebXClientConnector::stop() {
    if (this->_thread != NULL) {
        // Join thread and cleanup
        spdlog::info("Stopping client connector...");
        this->_thread->join();
        spdlog::info("Stopped client connector");
        delete this->_thread;
        this->_thread = NULL;
    }
}

void WebXClientConnector::mainLoop() {

    // Create client responder socket
    zmq::socket_t clientResponder = this->createClientResponder();

    // Create the event bus socket
    zmq::socket_t eventBus = this->createEventBusSubscriber();

#ifdef COMPILE_FOR_CPPZMQ_BEFORE_4_3_1
    zmq::pollitem_t pollItems [] = {
        { static_cast<void *>(eventBus), 0, ZMQ_POLLIN, 0 },
        { static_cast<void *>(clientResponder), 0, ZMQ_POLLIN, 0 }
    };
#else
    zmq::pollitem_t pollItems [] = {
        { eventBus, 0, ZMQ_POLLIN, 0 },
        { clientResponder, 0, ZMQ_POLLIN, 0 }
    };
#endif

    bool running = true;
    while (running) {
        zmq::message_t message;
        bool sendRequired = true;

        //  Wait for next message from client
        try {
#ifdef COMPILE_FOR_CPPZMQ_BEFORE_4_8_0
            zmq::poll(&pollItems[0], 2, -1);
#else
            zmq::poll(&pollItems[0], 2); // Defaults to timeout = -1
#endif

            // Check for event bus message
            if (pollItems[0].revents & ZMQ_POLLIN) {
#ifdef COMPILE_FOR_CPPZMQ_BEFORE_4_3_1
                bool retVal = eventBus.recv(&message);
#else
                auto retVal = eventBus.recv(message);
#endif
                std::string event = std::string(static_cast<char *>(message.data()), message.size());
                if (event == "shutdown") {
                    running = false;
                }
            }

            // Check for req-rep message
            if (pollItems[1].revents & ZMQ_POLLIN && running) {
#ifdef COMPILE_FOR_CPPZMQ_BEFORE_4_3_1
                bool retVal = clientResponder.recv(&message);
#else
                auto retVal = clientResponder.recv(message);
#endif
                
                if (retVal) {
                    std::string instruction = std::string(static_cast<char *>(message.data()), message.size());
                    spdlog::debug("Received {} message", instruction);
                    if (instruction == "comm") {
                        const std::string ports = fmt::format("{},{}", this->_publisherPort, this->_collectorPort);
                        zmq::message_t replyMessage(ports.c_str(), ports.size());
                        this->sendMessage(clientResponder, replyMessage);
                        sendRequired = false;

                    } else if (instruction == "ping") {
                        zmq::message_t replyMessage("pong", 4);
                        this->sendMessage(clientResponder, replyMessage);
                        sendRequired = false;

                    } else if (instruction.rfind("connect,", 0) == 0) {
                        std::string sessionId = instruction.substr(8);
                        if (sessionId == this->_sessionId) {
                            const WebXResult<std::pair<uint32_t, uint64_t>> result = this->_gateway.onClientConnect();
                            if (result.ok()) {
                                const std::string response = fmt::format("{:08x},{:016x}", result.data().first, result.data().second);
                                zmq::message_t replyMessage(response.c_str(), response.size());
                                this->sendMessage(clientResponder, replyMessage);
    
                            } else {
                                spdlog::warn("Failed to register client: {:s}", result.error());
                                zmq::message_t replyMessage(result.error().c_str(), result.error().size());
                                this->sendMessage(clientResponder, replyMessage);
                            }
    
                        } else {
                            spdlog::warn("Client not registed: session Id is incorrect");
                            zmq::message_t replyMessage("session id error", 16);
                            this->sendMessage(clientResponder, replyMessage);
                        }

                        sendRequired = false;

                    } else if (instruction.rfind("disconnect,", 0) == 0) {
                        std::string clientId = instruction.substr(11);
                        const WebXResult<uint32_t> hexToUint32 = HexUtils::toUint32(clientId);
                        if (hexToUint32.ok()) {
                            this->_gateway.onClientDisconnect(hexToUint32.data());
                        } else {
                            spdlog::warn("Cannot disconnect client as clientId is not valid hex {:s}", clientId);
                        }

                        // Send empty response
                        sendRequired = true;
                    
                    } else {
                        spdlog::warn("Message {:s} is unknown", instruction);
                    }
                }
            }
        
        } catch(zmq::error_t& e) {
            if (running) {
                spdlog::warn("WebXClientConnector interrupted from message recv: {:s}", e.what());
            }

        } catch (const std::exception& e) {
            spdlog::error("WebXClientConnector caught std::exception: {:s}", e.what());

        } catch (const std::string& e) {
            spdlog::error("WebXClientConnector caught std::string: {:s}", e.c_str());
        }

        if (sendRequired && running) {
            zmq::message_t replyMessage(0);
            this->sendMessage(clientResponder, replyMessage);
        }
    }
}

zmq::socket_t WebXClientConnector::createClientResponder() {
    //  Prepare our context and socket
    zmq::socket_t socket(*this->_context, ZMQ_REP);
#ifdef COMPILE_FOR_CPPZMQ_BEFORE_4_8_0
    int linger = 0;
    socket.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
#else
    socket.set(zmq::sockopt::linger, 0);
#endif

    try {
        socket.bind(this->_clientAddr);
        
        spdlog::debug("Client Responder socket bound to {:s}", this->_clientAddr);

        return socket;
    
    } catch (zmq::error_t& e) {
        spdlog::error("failed to bind Client Responder socket to {:s}", this->_clientAddr);
        exit(1);
    }    
}

zmq::socket_t WebXClientConnector::createEventBusSubscriber() {
    // Create the event bus socket
    zmq::socket_t eventBus(*this->_context, ZMQ_SUB);
#ifdef COMPILE_FOR_CPPZMQ_BEFORE_4_8_0
    eventBus.setsockopt(ZMQ_SUBSCRIBE, "", 0);
#else
    eventBus.set(zmq::sockopt::subscribe, "");
#endif

    try {
        eventBus.connect(this->_eventBusAddr);
        return eventBus;
    
    } catch (zmq::error_t& e) {
        spdlog::error("failed to connect Event Bus socket to {:s}", this->_eventBusAddr);
        exit(1);
    }
}

void WebXClientConnector::sendMessage(zmq::socket_t & clientResponder, zmq::message_t & message) {
#ifdef COMPILE_FOR_CPPZMQ_BEFORE_4_3_1
    clientResponder.send(message);
#else
    clientResponder.send(message, zmq::send_flags::none);
#endif            
}

