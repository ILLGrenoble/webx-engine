#include "WebXClientMessagePublisher.h"
#include "serializer/WebXBinarySerializer.h"
#include "WebXZMQ.h"
#include <spdlog/spdlog.h>

WebXClientMessagePublisher::WebXClientMessagePublisher() : 
    _thread(NULL),
    _running(false),
    _messageQueue(),
    _context(NULL),
    _port(0)  {
}

WebXClientMessagePublisher::~WebXClientMessagePublisher() {
    this->stop();
}

void WebXClientMessagePublisher::run(WebXBinarySerializer * serializer, zmq::context_t * context, int port) {
    std::lock_guard<std::mutex> lock(this->_mutex);
    this->_serializer = serializer;
    this->_context = context;
    this->_port = port;
    this->_running = true;
    if (this->_thread == NULL) {
        this->_thread = new std::thread(&WebXClientMessagePublisher::mainLoop, this);
    }
}

void WebXClientMessagePublisher::stop() {
    std::lock_guard<std::mutex> lock(this->_mutex);
    this->_running = false;
    this->_context = NULL;
    this->_port = 0;
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

    // Start zeroMQ publisher
    zmq::socket_t socket(*this->_context, ZMQ_PUB);
    int linger = 0;
    socket.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));

    char address[16];
    snprintf(address, sizeof(address) - 1, "tcp://*:%4d", this->_port);
    socket.bind(address);

    while (this->_running) {
        try {
           auto message = this->_messageQueue.get();
            if (message != NULL && this->_running) {

                zmq::message_t * replyMessage = this->_serializer->serialize(message);
#ifdef COMPILE_FOR_CPPZMQ_BEFORE_4_3_1
                socket.send(*replyMessage);
#else
                socket.send(*replyMessage, zmq::send_flags::none);
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
