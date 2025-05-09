#include "WebXClientInstructionSubscriber.h"
#include <models/instruction/WebXInstruction.h>
#include "WebXZMQ.h"
#include <spdlog/spdlog.h>

WebXClientInstructionSubscriber::WebXClientInstructionSubscriber(const WebXTransportSettings & settings, WebXGateway & gateway) : 
    _thread(NULL),
    _gateway(gateway),
    _eventBusAddr(settings.inprocEventBusAddress) {

    memcpy(this->_sessionId, settings.sessionId.data(), 16);
}

WebXClientInstructionSubscriber::~WebXClientInstructionSubscriber() {
    if (this->_thread != NULL) {
        delete this->_thread;
        this->_thread = NULL;
    }
}

void WebXClientInstructionSubscriber::run(zmq::context_t * context, const std::string & clientAddr, bool bindToClientAddr) {
    this->_context = context;
    this->_clientAddr = clientAddr;
    this->_bindToClientAddr = bindToClientAddr;
    if (this->_thread == NULL) {
        this->_thread = new std::thread(&WebXClientInstructionSubscriber::mainLoop, this);
    }
}

void WebXClientInstructionSubscriber::stop() {
    if (this->_thread != NULL) {
        // Join thread and cleanup
        spdlog::info("Stopping client instruction subscriber...");
        this->_thread->join();
        spdlog::info("Stopped client instruction subscriber");
        delete this->_thread;
        this->_thread = NULL;
    }    
}

void WebXClientInstructionSubscriber::mainLoop() {

    // Create client instruction socket
    zmq::socket_t clientInstructionSocket = this->createClientInstructionSubscriber();

    // Create the event bus socket
    zmq::socket_t eventBus = this->createEventBusSubscriber();

#ifdef COMPILE_FOR_CPPZMQ_BEFORE_4_3_1
    zmq::pollitem_t pollItems [] = {
        { static_cast<void *>(eventBus), 0, ZMQ_POLLIN, 0 },
        { static_cast<void *>(clientInstructionSocket), 0, ZMQ_POLLIN, 0 }
    };
#else
    zmq::pollitem_t pollItems [] = {
        { eventBus, 0, ZMQ_POLLIN, 0 },
        { clientInstructionSocket, 0, ZMQ_POLLIN, 0 }
    };
#endif

    bool running = true;
    while (running) {
        zmq::message_t message;

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

            // Check for published client instruction
            if (pollItems[1].revents & ZMQ_POLLIN && running) {
#ifdef COMPILE_FOR_CPPZMQ_BEFORE_4_3_1
                bool retVal = clientInstructionSocket.recv(&message);
#else
                auto retVal = clientInstructionSocket.recv(message);
#endif
                if (retVal) {
                    // Deserialize instruction
                    auto instruction = this->_decoder.decode((const unsigned char *)message.data(), message.size());
                    if (instruction != NULL) {
                        // spdlog::debug("Received instruction type {}", (int)instruction->type);
                        // Handle message
                        this->_gateway.handleInstruction(instruction);
                    }
                }
            }
        
        } catch(zmq::error_t& e) {
            if (running) {
                spdlog::warn("WebXClientInstructionSubscriber interrupted from message recv: {:s}", e.what());
            }

        } catch (const std::exception& e) {
            spdlog::error("WebXClientInstructionSubscriber caught std::exception: {:s}", e.what());

        } catch (const std::string& e) {
            spdlog::error("WebXClientInstructionSubscriber caught std::string: {:s}", e.c_str());
        }
    }
}

zmq::socket_t WebXClientInstructionSubscriber::createClientInstructionSubscriber() {
    // Create instruction subscriber
    zmq::socket_t socket(*this->_context, ZMQ_SUB);

#ifdef COMPILE_FOR_CPPZMQ_BEFORE_4_8_0
    int linger = 0;
    socket.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
#else
    socket.set(zmq::sockopt::linger, 0);
#endif

    try {
        if (this->_bindToClientAddr) {
            // Listen to all instructions
#ifdef COMPILE_FOR_CPPZMQ_BEFORE_4_8_0
            socket.setsockopt(ZMQ_SUBSCRIBE, "", 0);
#else
            socket.set(zmq::sockopt::subscribe, "");
#endif
            socket.bind(this->_clientAddr);

        } else {
            // Subscribe only to instructions that have the correct sessionId
#ifdef COMPILE_FOR_CPPZMQ_BEFORE_4_8_0
            socket.setsockopt(ZMQ_SUBSCRIBE, this->_sessionId, 16);
#else
            std::string sessionId(reinterpret_cast<char*>(this->_sessionId), 16);
            socket.set(zmq::sockopt::subscribe, sessionId);
#endif
            socket.connect(this->_clientAddr);
        }

        spdlog::debug("Client Instruction socket {} to {:s}", this->_bindToClientAddr ? "bound" : "connected", this->_clientAddr);

        return socket;
    
    } catch (zmq::error_t& e) {
        spdlog::error("failed to {} Client Instruction socket to {:s}", this->_bindToClientAddr ? "bind" : "connect", this->_clientAddr);
        exit(1);
    }
} 

zmq::socket_t WebXClientInstructionSubscriber::createEventBusSubscriber() {
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
