#include "WebXClientConnector.h"
#include "WebXClientMessagePublisher.h"
#include "WebXClientCommandCollector.h"
#include <message/WebXScreenMessage.h>
#include <message/WebXWindowsMessage.h>
#include <message/WebXImageMessage.h>
#include <message/WebXCursorImageMessage.h>
#include <message/WebXVoidMessage.h>
#include <instruction/WebXImageInstruction.h>
#include <instruction/WebXMouseInstruction.h>
#include <instruction/WebXCursorImageInstruction.h>
#include <serializer/WebXBinarySerializer.h>
#include <display/WebXManager.h>
#include <display/WebXDisplay.h>
#include <display/WebXController.h>
#include <input/WebXMouse.h>
#include <utils/WebXBinaryBuffer.h>
#include <unistd.h>
#include <string>
#include <spdlog/spdlog.h>
#include <X11/Xlib.h>

WebXClientConnector * WebXClientConnector::_instance = NULL;
int WebXClientConnector::CONNECTOR_PORT = 5555;
int WebXClientConnector::COLLECTOR_PORT = 5556;
int WebXClientConnector::PUBLISHER_PORT = 5557;

WebXClientConnector::WebXClientConnector() :
    _publisher(NULL),
    _collector(NULL),
    _serializer(new WebXBinarySerializer()),
    _running(false) {
    std::signal(SIGINT, WebXClientConnector::signalHandler);
}

WebXClientConnector::~WebXClientConnector() {
    if (this->_publisher) {
        delete _publisher;
        _publisher = NULL;
    }
    if (this->_collector) {
        _collector = NULL;
    }
    if (this->_serializer) {
        delete _serializer;
        _serializer = NULL;
    }
}

WebXClientConnector * WebXClientConnector::initInstance() {
    if (_instance == NULL) {
        _instance = new WebXClientConnector();
        _instance->init();
    }
    return _instance;
}

void WebXClientConnector::init() {
    this->_publisher = new WebXClientMessagePublisher();
    WebXManager::instance()->getController()->setConnection(this->_publisher);

    this->_collector = new WebXClientCommandCollector();
}

void WebXClientConnector::run(int socketTimoutMs) {
    //  Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REP);
    int linger = 0;
    socket.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
    socket.setsockopt(ZMQ_RCVTIMEO, &socketTimoutMs, sizeof(socketTimoutMs));

    char address[16];
    snprintf(address, sizeof(address) - 1, "tcp://*:%4d", WebXClientConnector::CONNECTOR_PORT);
    socket.bind(address);

    this->_publisher->run(this->_serializer, &context, WebXClientConnector::PUBLISHER_PORT);
    this->_collector->run(this->_serializer, &context, WebXClientConnector::COLLECTOR_PORT);

    this->_running = true;
    while (this->_running) {
        zmq::message_t instructionMessage;
        bool sendRequired = false;

        //  Wait for next message from client
        try {
#ifdef COMPILE_FOR_CPPZMQ_BEFORE_4_3_1
            bool retVal = socket.recv(&instructionMessage);
#else
            auto retVal = socket.recv(instructionMessage);
#endif
            bool sendRequired = true;
            if (retVal) {
                const unsigned char * instructionData = (const unsigned char *)instructionMessage.data();

                // std::string instructionString;
                // instructionString.assign(instructionData, instructionMessage.size());
                // spdlog::trace("Received instruction {:s}", instructionString.c_str());

                if (instructionMessage.size() == 4 && strncmp((const char *) instructionData, "comm", 4) == 0) {
                    const std::string ports = fmt::format("{},{}", WebXClientConnector::PUBLISHER_PORT, WebXClientConnector::COLLECTOR_PORT);
                    zmq::message_t replyMessage(ports.c_str(), ports.size());
                    this->sendMessage(socket, replyMessage);
                    sendRequired = false;
                }
            }
        
        } catch(zmq::error_t& e) {
            if (this->_running) {
                spdlog::warn("WebXClientConnector interrupted from message recv: {:s}", e.what());
            }
        } catch (const std::exception& e) {
            spdlog::error("WebXClientConnector caught std::exception: {:s}", e.what());
        } catch (const std::string& e) {
            spdlog::error("WebXClientConnector caught std::string: {:s}", e.c_str());
        // } catch (const WebXBinaryBuffer::OverflowException & e) {
        //     spdlog::error("WebXClientConnector caught WebXBinaryBuffer::OverflowException: offset: {:d}, dataLength: {:d}, bufferLength: {:d}", e.offset, e.dataSize, e.bufferLength);
        }

        if (sendRequired) {
            zmq::message_t replyMessage(0);
            this->sendMessage(socket, replyMessage);
        }
    }

    // Suicide
    delete this;

    spdlog::info("Stopped client connector");
}

void WebXClientConnector::sendMessage(zmq::socket_t & socket, zmq::message_t & message) {
#ifdef COMPILE_FOR_CPPZMQ_BEFORE_4_3_1
    socket.send(message);
#else
    socket.send(message, zmq::send_flags::none);
#endif
}

void WebXClientConnector::stop() {
    spdlog::info("Stopping client connector...");
    WebXManager::instance()->getController()->removeConnection();

    this->_running = false;

    if (this->_collector) {
        this->_collector->stop();
    }
}

void WebXClientConnector::shutdown() {
    spdlog::info("Shutdown");

    if (_instance) {
        _instance->stop();
        _instance = NULL;
    }

    WebXManager::instance()->shutdown();
}

