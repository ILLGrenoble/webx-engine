#include "WebXClientMessagePublisher.h"
#include "WebXZMQ.h"
#include <spdlog/spdlog.h>

WebXClientMessagePublisher::WebXClientMessagePublisher(const WebXTransportSettings & settings, const WebXBinarySerializer & serializer) :
    _thread(NULL),
    _running(false),
    _messageQueue(),
    _serializer(serializer),
    _eventBusAddr(settings.inprocEventBusAddress) {
}

WebXClientMessagePublisher::~WebXClientMessagePublisher() {
    this->stop();
}

void WebXClientMessagePublisher::run(zmq::context_t * context, const std::string & clientAddr, bool bindToClientAddr) {
    this->_context = context;
    this->_clientAddr = clientAddr;
    this->_bindToClientAddr = bindToClientAddr;
    this->_running = true;
    if (this->_thread == NULL) {
        this->_thread = new std::thread(&WebXClientMessagePublisher::mainLoop, this);
    }
}

void WebXClientMessagePublisher::stop() {
    this->_running = false;
    if (this->_thread != NULL) {
        // Join thread and cleanup
        spdlog::info("Stopping client message publisher...");
        this->_messageQueue.stop();
        this->_thread->join();
        spdlog::info("Stopped client message publisher");
        delete this->_thread;
        this->_thread = NULL;
    }
}

void WebXClientMessagePublisher::mainLoop() {

    // Create client message publisher socket
    zmq::socket_t messagePublisher = this->createClientMessagePublisher();

    while (this->_running) {
        try {
           auto message = this->_messageQueue.get();
            if (message != NULL && this->_running) {

                zmq::message_t * replyMessage = this->_serializer.serialize(message);
#ifdef COMPILE_FOR_CPPZMQ_BEFORE_4_3_1
                messagePublisher.send(*replyMessage);
#else
                messagePublisher.send(*replyMessage, zmq::send_flags::none);
#endif
                delete replyMessage;
            }

        } catch(zmq::error_t& e) {
            spdlog::warn("WebXClientMessagePublisher interrupted from message send: {:s}", e.what());

        } catch (const std::exception& e) {
            spdlog::error("WebXClientMessagePublisher caught std::exception: {:s}", e.what());

        } catch (const std::string& e) {
            spdlog::error("WebXClientMessagePublisher caught std::string: {:s}", e.c_str());
        }
    }
}

zmq::socket_t WebXClientMessagePublisher::createClientMessagePublisher() {
    // Start zeroMQ publisher
    zmq::socket_t socket(*this->_context, ZMQ_PUB);
#ifdef COMPILE_FOR_CPPZMQ_BEFORE_4_8_0
    int linger = 0;
    socket.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
#else
    socket.set(zmq::sockopt::linger, 0);
#endif

    try {
        if (this->_bindToClientAddr) {
            socket.bind(this->_clientAddr);
        } else {
            socket.connect(this->_clientAddr);
        }

        spdlog::debug("Client Message Publisher socket {} to {:s}", this->_bindToClientAddr ? "bound" : "connected", this->_clientAddr);

        return socket;
    
    } catch (zmq::error_t& e) {
        spdlog::error("failed to {} Client Message Publisher socket to {:s}", this->_bindToClientAddr ? "bind" : "connect", this->_clientAddr);
        exit(1);
    }    
}

