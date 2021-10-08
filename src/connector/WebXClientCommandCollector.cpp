#include "WebXClientCommandCollector.h"
#include "instruction/WebXInstruction.h"
#include "serializer/WebXSerializer.h"
#include "serializer/WebXSerializer.h"
#include <display/WebXManager.h>
#include <display/WebXController.h>
#include "WebXZMQ.h"
#include <spdlog/spdlog.h>

WebXClientCommandCollector::WebXClientCommandCollector() : 
    _thread(NULL),
    _running(false),
    _context(NULL),
    _port(0) {
}

WebXClientCommandCollector::~WebXClientCommandCollector() {
    this->_context = NULL;
    this->_port = 0;
    if (this->_thread != NULL) {
        delete this->_thread;
        this->_thread = NULL;
    }
}

void WebXClientCommandCollector::run(WebXSerializer * serializer, zmq::context_t * context, int port) {
    tthread::lock_guard<tthread::mutex> lock(this->_mutex);
    this->_serializer = serializer;
    this->_context = context;
    this->_port = port;
    this->_running = true;
    if (this->_thread == NULL) {
        this->_thread = new tthread::thread(WebXClientCommandCollector::threadMain, (void *)this);
    }
}

void WebXClientCommandCollector::stop() {
    spdlog::info("Stopping client command collector...");
    tthread::lock_guard<tthread::mutex> lock(this->_mutex);
    this->_running = false;
}

void WebXClientCommandCollector::threadMain(void * arg) {
    WebXClientCommandCollector * self  = (WebXClientCommandCollector *)arg;
    self->mainLoop();
}

void WebXClientCommandCollector::mainLoop() {

    // Start zeroMQ publisher
    zmq::socket_t socket(*this->_context, ZMQ_PULL);
    int linger = 0;
    socket.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));

    char address[16];
    snprintf(address, sizeof(address) - 1, "tcp://*:%4d", this->_port);
    socket.bind(address);

    while (this->_running) {
        zmq::message_t instructionMessage;

        try {
#ifdef COMPILE_FOR_CPPZMQ_BEFORE_4_3_1
            bool retVal = socket.recv(&instructionMessage);
#else
            auto retVal = socket.recv(instructionMessage);
#endif
            if (retVal) {
                // Deserialize instruction
                auto instruction = this->_serializer->deserialize(instructionMessage.data(), instructionMessage.size());
                if (instruction != NULL) {
                    // Handle message
                    WebXManager::instance()->getController()->onClientInstruction(instruction);
                }
            }
        
        } catch(zmq::error_t& e) {
            if (this->_running) {
                spdlog::warn("WebXClientCommandCollector interrupted from message recv: {:s}", e.what());
            }
        } catch (const std::exception& e) {
            spdlog::error("WebXClientCommandCollector caught std::exception: {:s}", e.what());
        } catch (const std::string& e) {
            spdlog::error("WebXClientCommandCollector caught std::string: {:s}", e.c_str());
        }
    }

    // Suicide
    // delete this;

    spdlog::info("Stopped client command collector");
}
