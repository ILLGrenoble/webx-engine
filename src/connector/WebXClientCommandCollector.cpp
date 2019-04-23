#include "WebXClientCommandCollector.h"
#include "instruction/WebXInstruction.h"
#include <spdlog/spdlog.h>

WebXClientCommandCollector::WebXClientCommandCollector() : 
    _thread(NULL),
    _running(false),
    _commandQueue(new WebXQueue<WebXInstruction>(1024)),
    _context(NULL),
    _port(0) {
}

WebXClientCommandCollector::~WebXClientCommandCollector() {
    this->stop();
}

void WebXClientCommandCollector::run(zmq::context_t * context, int port) {
    tthread::lock_guard<tthread::mutex> lock(this->_mutex);
    this->_context = context;
    this->_port = port;
    this->_running = true;
    if (this->_thread == NULL) {
        this->_thread = new tthread::thread(WebXClientCommandCollector::threadMain, (void *)this);
    }
}

void WebXClientCommandCollector::stop() {
    tthread::lock_guard<tthread::mutex> lock(this->_mutex);
    this->_running = false;
    this->_context = NULL;
    this->_port = 0;
    if (this->_thread != NULL) {
        // Join thread and cleanup
        spdlog::info("Stopping client command collector...");
        this->_commandQueue->stop();
        this->_thread->join();
        spdlog::info("Stopped client command collector");
        delete this->_thread;
        this->_thread = NULL;
    }
}

void WebXClientCommandCollector::threadMain(void * arg) {
    WebXClientCommandCollector * self  = (WebXClientCommandCollector *)arg;
    self->mainLoop();
}

void WebXClientCommandCollector::mainLoop() {
    while (this->_running) {
        WebXInstruction * message = this->_commandQueue->get();
        if (this->_running) {

        }

        if (message != NULL) {
            delete message;
        }
    }
}
