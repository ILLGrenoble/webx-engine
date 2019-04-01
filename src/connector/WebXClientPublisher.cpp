#include "WebXClientPublisher.h"
#include "WebXMessage.h"

WebXClientPublisher::WebXClientPublisher() : 
    _thread(NULL),
    _running(false),
    _messageQueue(new WebXQueue<WebXMessage>(1024)) {
}

WebXClientPublisher::~WebXClientPublisher() {
    this->stop();
}

void WebXClientPublisher::run() {
    tthread::lock_guard<tthread::mutex> lock(this->_mutex);
    this->_running = true;
    if (this->_thread == NULL) {
        this->_thread = new tthread::thread(WebXClientPublisher::threadMain, (void *)this);
    }
}

void WebXClientPublisher::stop() {
    tthread::lock_guard<tthread::mutex> lock(this->_mutex);
    this->_running = false;
    if (this->_thread != NULL) {
        // Join thread and cleanup
        printf("Stopping client publisher...\n");
        this->_messageQueue->stop();
        this->_thread->join();
        printf("... stopped client publisher\n");
        delete this->_thread;
        this->_thread = NULL;
    }
}

void WebXClientPublisher::onDisplayChanged(const std::vector<WebXWindow *> & windows) {
    // Create new message
    // this->_messageQueue->put(message);
}

void WebXClientPublisher::threadMain(void * arg) {
    WebXClientPublisher * self  = (WebXClientPublisher *)arg;
    self->mainLoop();
}

void WebXClientPublisher::mainLoop() {
    while (this->_running) {
        WebXMessage * message = this->_messageQueue->get();
        if (this->_running) {
            // ZeroMQ publish data
        }

        if (message != NULL) {
            delete message;
        }
    }
}
