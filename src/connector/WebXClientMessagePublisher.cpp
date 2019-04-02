#include "WebXClientMessagePublisher.h"
#include "message/WebXWindowsMessage.h"
#include <zmq.hpp>

WebXClientMessagePublisher::WebXClientMessagePublisher() : 
    _thread(NULL),
    _running(false),
    _messageQueue(new WebXQueue<WebXMessage>(1024)),
    _context(NULL),
    _port(0)  {
}

WebXClientMessagePublisher::~WebXClientMessagePublisher() {
    this->stop();
}

void WebXClientMessagePublisher::run(zmq::context_t * context, int port) {
    tthread::lock_guard<tthread::mutex> lock(this->_mutex);
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
        printf("Stopping client message publisher...\n");
        this->_messageQueue->stop();
        this->_thread->join();
        printf("... stopped client message publisher\n");
        delete this->_thread;
        this->_thread = NULL;
    }
}

void WebXClientMessagePublisher::onDisplayChanged(const std::vector<WebXWindow *> & windows) {
    // Create new message
    WebXWindowsMessage * message = new WebXWindowsMessage(windows);
    this->_messageQueue->put(message);
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
        WebXMessage * message = this->_messageQueue->get();
        if (message != NULL && this->_running) {
            // ZeroMQ publish data
            const nlohmann::json & jMessage = message->getJson();
            std::string messageData = jMessage.dump();

            zmq::message_t replyMessage(messageData.size());
            memcpy(replyMessage.data(), messageData.c_str(), messageData.size());
            socket.send(replyMessage);

            delete message;
        }
    }
}
