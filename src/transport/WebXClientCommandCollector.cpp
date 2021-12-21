#include "WebXClientCommandCollector.h"
#include "instruction/WebXInstruction.h"
#include "serializer/WebXBinarySerializer.h"
#include <controller/WebXController.h>
#include <utils/WebXSettings.h>
#include "WebXZMQ.h"
#include <spdlog/spdlog.h>

WebXClientCommandCollector::WebXClientCommandCollector() : 
    _thread(NULL) {
}

WebXClientCommandCollector::~WebXClientCommandCollector() {
    if (this->_thread != NULL) {
        delete this->_thread;
        this->_thread = NULL;
    }
}

void WebXClientCommandCollector::run(WebXBinarySerializer * serializer, zmq::context_t * context, const std::string & clientAddr, bool bindToClientAddr, const std::string & eventBusAddr, WebXSettings * settings) {
    this->_serializer = serializer;
    this->_context = context;
    this->_clientAddr = clientAddr;
    this->_bindToClientAddr = bindToClientAddr;
    this->_eventBusAddr = eventBusAddr;
    memcpy(this->_sessionId, settings->sessionId, 16);
    if (this->_thread == NULL) {
        this->_thread = new std::thread(&WebXClientCommandCollector::mainLoop, this);
    }
}

void WebXClientCommandCollector::stop() {
    if (this->_thread != NULL) {
        // Join thread and cleanup
        spdlog::info("Stopping client command collector...");
        this->_thread->join();
        spdlog::info("Stopped client command collector");
        delete this->_thread;
        this->_thread = NULL;
    }    
}

void WebXClientCommandCollector::mainLoop() {

    // Create client instruction socket
    zmq::socket_t clientInstructionSocket = this->createClientInstructionSubscriber();

    // Create the event bus socket
    zmq::socket_t eventBus = this->createEventBusSubscriber();

    zmq::pollitem_t pollItems [] = {
        { eventBus, 0, ZMQ_POLLIN, 0 },
        { clientInstructionSocket, 0, ZMQ_POLLIN, 0 }
    };

    bool running = true;
    while (running) {
        zmq::message_t message;

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

            // Check for published client instruction
            if (pollItems[1].revents & ZMQ_POLLIN && running) {
#ifdef COMPILE_FOR_CPPZMQ_BEFORE_4_3_1
                bool retVal = clientInstructionSocket.recv(&message);
#else
                auto retVal = clientInstructionSocket.recv(message);
#endif
                if (retVal) {
                    // Deserialize instruction
                    auto instruction = this->_serializer->deserialize((const unsigned char *)message.data(), message.size());
                    if (instruction != NULL) {
                        spdlog::debug("Received instruction type {}", instruction->type);
                        // Handle message
                        WebXController::instance()->onClientInstruction(instruction);
                    }
                }
            }
        
        } catch(zmq::error_t& e) {
            if (running) {
                spdlog::warn("WebXClientCommandCollector interrupted from message recv: {:s}", e.what());
            }

        } catch (const std::exception& e) {
            spdlog::error("WebXClientCommandCollector caught std::exception: {:s}", e.what());

        } catch (const std::string& e) {
            spdlog::error("WebXClientCommandCollector caught std::string: {:s}", e.c_str());
        }
    }
}

zmq::socket_t WebXClientCommandCollector::createClientInstructionSubscriber() {
    // Create instruction subscriber
    zmq::socket_t socket(*this->_context, ZMQ_SUB);
    int linger = 0;
    socket.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));

    try {
        if (this->_bindToClientAddr) {
            // Listen to all instructions
            socket.setsockopt(ZMQ_SUBSCRIBE, "", 0);
            socket.bind(this->_clientAddr);

        } else {
            // Subscribe only to instructions that have the correct sessionId
            socket.setsockopt(ZMQ_SUBSCRIBE, this->_sessionId, 16);
            socket.connect(this->_clientAddr);
        }

        spdlog::debug("Client Instruction socket {} to {:s}", this->_bindToClientAddr ? "bound" : "connected", this->_clientAddr);

        return socket;
    
    } catch (zmq::error_t& e) {
        spdlog::error("failed to {} Client Instruction socket to {:s}", this->_bindToClientAddr ? "bind" : "connect", this->_clientAddr);
        exit(1);
    }
} 

zmq::socket_t WebXClientCommandCollector::createEventBusSubscriber() {
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
