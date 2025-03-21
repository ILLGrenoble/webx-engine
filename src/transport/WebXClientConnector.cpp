#include "WebXClientConnector.h"
#include <utils/WebXHexUtils.h>
#include <utils/WebXStringUtils.h>
#include <unistd.h>
#include <string>
#include <spdlog/spdlog.h>

WebXClientConnector::WebXClientConnector(const WebXTransportSettings & settings, WebXGateway & gateway) :
    _thread(NULL),
    _gateway(gateway),
    _publisherPort(settings.publisherPort),
    _collectorPort(settings.collectorPort),
    _sessionId(settings.sessionIdString),
    _socketState(WAITING_RECV) {
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

            if (this->_socketState != WAITING_RECV) {
                spdlog::error("Client Connected socket not waiting to receive");
                exit(1);
            }

            // Check for req-rep message
            if (pollItems[1].revents & ZMQ_POLLIN && running) {
#ifdef COMPILE_FOR_CPPZMQ_BEFORE_4_3_1
                bool retVal = clientResponder.recv(&message);
#else
                auto retVal = clientResponder.recv(message);
#endif
                this->_socketState = WAITING_SEND;
                if (retVal) {
                    std::string instruction = std::string(static_cast<char *>(message.data()), message.size());
//                    spdlog::debug("Received {} message", instruction);
                    if (instruction == "comm") {
                        const std::string ports = fmt::format("{},{}", this->_publisherPort, this->_collectorPort);
                        this->sendMessage(clientResponder, ports);

                    } else if (instruction.rfind("ping", 0) == 0) {
                        std::vector<std::string> elements = StringUtils::split(instruction, ',');
                        if (elements.size() == 1) {
                            // Standard/legacy ping command
                            this->sendMessage(clientResponder, "pong");

                        } else {
                            const std::string & sessionId = elements[1];
                            if (sessionId != this->_sessionId) {
                                spdlog::warn("Received ping with an invalid session Id: {}", sessionId);
                                const std::string response = fmt::format("pang,{},Invalid session Id", sessionId);
                                this->sendMessage(clientResponder, response);
    
                            } else {
                                const std::string response = fmt::format("pong,{}", sessionId);
                                this->sendMessage(clientResponder, response);
                            }
                        }

                    } else if (instruction.rfind("connect", 0) == 0) {
                        std::vector<std::string> elements = StringUtils::split(instruction, ',');
                        if (elements.size() != 2) {
                            spdlog::warn("Invalid connect command received: {}", instruction);
                            this->sendMessage(clientResponder, "invalid connect command");

                        } else {
                            const std::string & sessionId = elements[1];
                            const std::string & response = this->connectClient(sessionId);
                            this->sendMessage(clientResponder, response);
                        }

                    } else if (instruction.rfind("disconnect", 0) == 0) {
                        std::vector<std::string> elements = StringUtils::split(instruction, ',');
                        if (elements.size() != 3) {
                            spdlog::warn("Invalid disconnect command received: {}", instruction);
                            this->sendMessage(clientResponder, "invalid disconnect command");

                        } else {
                            const std::string & sessionId = elements[1];
                            const std::string & clientIdString = elements[2];
                            const std::string & response = this->disconnectClient(sessionId, clientIdString);
                            this->sendMessage(clientResponder, response);
                        }

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

        if (this->_socketState == WAITING_SEND && running) {
            this->sendMessage(clientResponder, "");
        }
    }
}

std::string WebXClientConnector::connectClient(const std::string & sessionId) {
    if (sessionId == this->_sessionId) {
        const WebXResult<std::pair<uint32_t, uint64_t>> result = this->_gateway.onClientConnect();
        if (result.ok()) {
            const std::string response = fmt::format("{:08x},{:016x}", result.data().first, result.data().second);
            return response;

        } else {
            spdlog::warn("Failed to register client: {:s}", result.error());
            return result.error();
        }

    } else {
        spdlog::warn("Client not connected: session Id is incorrect");
        return "session id error";
    }
}

std::string WebXClientConnector::disconnectClient(const std::string & sessionId, const std::string & clientIdString) {
    if (sessionId == this->_sessionId) {
        const WebXResult<uint32_t> clientId = HexUtils::toUint32(clientIdString);
        if (clientId.ok()) {
            const WebXResult<void> result = this->_gateway.onClientDisconnect(clientId.data());
            if (result.ok()) {
                return "success";

            } else {
                spdlog::warn("Failed to disconnect client: {:s}", result.error());
                return result.error();
            }

        } else {
            spdlog::warn("Cannot disconnect client as clientId is not valid hex {:s}", clientIdString);
            return "client id is invalid";
        }

    } else {
        spdlog::warn("Client not disconnected: session Id is incorrect");
        return "session id error";
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

void WebXClientConnector::sendMessage(zmq::socket_t & clientResponder, const std::string & messageText) {
    zmq::message_t message(messageText.c_str(), messageText.length());
#ifdef COMPILE_FOR_CPPZMQ_BEFORE_4_3_1
    clientResponder.send(message);
#else
    clientResponder.send(message, zmq::send_flags::none);
#endif
    this->_socketState = WAITING_RECV;
}

