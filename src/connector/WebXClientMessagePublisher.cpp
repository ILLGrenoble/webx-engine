#include "WebXClientMessagePublisher.h"
#include "message/WebXWindowsMessage.h"
#include "message/WebXImageMessage.h"
#include "message/WebXSubImagesMessage.h"
#include "message/WebXMouseMessage.h"
#include "serializer/WebXBinarySerializer.h"
#include "WebXZMQ.h"
#include <spdlog/spdlog.h>

WebXClientMessagePublisher::WebXClientMessagePublisher() : 
    _thread(NULL),
    _running(false),
    _messageQueue(1024),
    _context(NULL),
    _port(0)  {
}

WebXClientMessagePublisher::~WebXClientMessagePublisher() {
    this->stop();
}

void WebXClientMessagePublisher::run(WebXBinarySerializer * serializer, zmq::context_t * context, int port) {
    tthread::lock_guard<tthread::mutex> lock(this->_mutex);
    this->_serializer = serializer;
    this->_context = context;
    this->_port = port;
    this->_running = true;
    if (this->_thread == NULL) {
        this->_thread = new tthread::thread(WebXClientMessagePublisher::threadMain, (void *)this);
    }
}

void WebXClientMessagePublisher::stop() {
    tthread::lock_guard<tthread::mutex> lock(this->_mutex);
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

void WebXClientMessagePublisher::onDisplayChanged(const std::vector<WebXWindowProperties> & windows) {
    auto message = std::make_shared<WebXWindowsMessage>(windows);
    this->_messageQueue.put(message);
}

void WebXClientMessagePublisher::onImageChanged(unsigned long windowId, std::shared_ptr<WebXImage> image) {
    auto message = std::make_shared<WebXImageMessage>(windowId, image);
    this->_messageQueue.put(message);
}

void WebXClientMessagePublisher::onSubImagesChanged(unsigned long windowId, const std::vector<WebXSubImage> & subImages) {
    auto message = std::make_shared<WebXSubImagesMessage>(windowId, subImages);
    this->_messageQueue.put(message);
}

void WebXClientMessagePublisher::onMouseChanged(int x, int y, uint32_t cursorId) {
    auto message = std::make_shared<WebXMouseMessage>(x, y, cursorId);
    this->_messageQueue.put(message);
}

void WebXClientMessagePublisher::threadMain(void * arg) {
    WebXClientMessagePublisher * self  = (WebXClientMessagePublisher *)arg;
    self->mainLoop();
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
