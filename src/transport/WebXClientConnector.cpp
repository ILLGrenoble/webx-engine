#include "WebXClientConnector.h"
#include <utils/WebXSettings.h>
#include <unistd.h>
#include <string>
#include <spdlog/spdlog.h>

WebXClientConnector::WebXClientConnector() :
    _thread(NULL) {
}

WebXClientConnector::~WebXClientConnector() {
    if (this->_thread != NULL) {
        delete this->_thread;
        this->_thread = NULL;
    }
}

void WebXClientConnector::run(WebXBinarySerializer * serializer, zmq::context_t * context, const std::string & clientAddr, const std::string & eventBusAddr, WebXSettings * settings) {
    this->_serializer = serializer;
    this->_context = context;
    this->_clientAddr = clientAddr;
    this->_eventBusAddr = eventBusAddr;
    this->_publisherPort = settings->publisherPort;
    this->_collectorPort = settings->collectorPort;
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

    zmq::pollitem_t pollItems [] = {
        { eventBus, 0, ZMQ_POLLIN, 0 },
        { clientResponder, 0, ZMQ_POLLIN, 0 }
    };

    bool running = true;
    while (running) {
        zmq::message_t message;
        bool sendRequired = false;

        //  Wait for next message from client
        try {
            zmq::poll(&pollItems[0], 2, -1);

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
                bool sendRequired = true;
                if (retVal) {
                    std::string instruction = std::string(static_cast<char *>(message.data()), message.size());
                    spdlog::debug("Received {} message", instruction);
                    if (instruction == "comm") {
                        const std::string ports = fmt::format("{},{}", this->_publisherPort, this->_collectorPort);
                        zmq::message_t replyMessage(ports.c_str(), ports.size());
#ifdef COMPILE_FOR_CPPZMQ_BEFORE_4_3_1
                        clientResponder.send(replyMessage);
#else
                        clientResponder.send(replyMessage, zmq::send_flags::none);
#endif            
                        sendRequired = false;
                    } else if (instruction == "ping") {
#ifdef COMPILE_FOR_CPPZMQ_BEFORE_4_3_1
                        clientResponder.send(replyMessage);
#else
                        zmq::message_t replyMessage("pong", 4);
                        clientResponder.send(replyMessage, zmq::send_flags::none);
#endif            
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
#ifdef COMPILE_FOR_CPPZMQ_BEFORE_4_3_1
            clientResponder.send(replyMessage);
#else
            clientResponder.send(replyMessage, zmq::send_flags::none);
#endif            
        }
    }
}

zmq::socket_t WebXClientConnector::createClientResponder() {
    //  Prepare our context and socket
    zmq::socket_t socket(*this->_context, ZMQ_REP);
    int linger = 0;
    socket.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));

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
    eventBus.setsockopt(ZMQ_SUBSCRIBE, "", 0);

    try {
        eventBus.connect(this->_eventBusAddr);
        return eventBus;
    
    } catch (zmq::error_t& e) {
        spdlog::error("failed to connect Event Bus socket to {:s}", this->_eventBusAddr);
        exit(1);
    }
}


