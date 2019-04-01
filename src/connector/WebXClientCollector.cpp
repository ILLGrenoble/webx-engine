#include "WebXClientCollector.h"
#include "WebXMessage.h"

WebXClientCollector::WebXClientCollector() : 
    _thread(NULL),
    _running(false),
    _messageQueue(new WebXQueue<WebXMessage>(1024)) {
}

WebXClientCollector::~WebXClientCollector() {
    this->stop();
}

void WebXClientCollector::run() {
    tthread::lock_guard<tthread::mutex> lock(this->_mutex);
    this->_running = true;
    if (this->_thread == NULL) {
        this->_thread = new tthread::thread(WebXClientCollector::threadMain, (void *)this);
    }
}

void WebXClientCollector::stop() {
    tthread::lock_guard<tthread::mutex> lock(this->_mutex);
    this->_running = false;
    if (this->_thread != NULL) {
        // Join thread and cleanup
        printf("Stopping client collector...\n");
        this->_messageQueue->stop();
        this->_thread->join();
        printf("... stopped client collector\n");
        delete this->_thread;
        this->_thread = NULL;
    }
}

void WebXClientCollector::threadMain(void * arg) {
    WebXClientCollector * self  = (WebXClientCollector *)arg;
    self->mainLoop();
}

void WebXClientCollector::mainLoop() {
    while (this->_running) {
        WebXMessage * message = this->_messageQueue->get();
        if (this->_running) {

        }

        if (message != NULL) {
            delete message;
        }
    }
}
