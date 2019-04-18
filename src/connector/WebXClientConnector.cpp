#include "WebXClientConnector.h"
#include "message/WebXConnectionMessage.h"
#include "message/WebXWindowsMessage.h"
#include "message/WebXImageMessage.h"
#include "serializer/WebXJsonSerializer.h"
#include "WebXClientMessagePublisher.h"
#include "WebXClientCommandCollector.h"
#include <display/WebXManager.h>
#include <display/WebXDisplay.h>
#include <display/WebXController.h>
#include <unistd.h>
#include <string>
#include <zmq.hpp>

WebXClientConnector * WebXClientConnector::_instance = NULL;
int WebXClientConnector::CONNECTOR_PORT = 5555;
int WebXClientConnector::COLLECTOR_PORT = 5556;
int WebXClientConnector::PUBLISHER_PORT = 5557;

WebXClientConnector::WebXClientConnector() :
    _publisher(NULL),
    _collector(NULL),
    _serializer(new WebXJsonSerializer()),
    _running(false) {
    std::signal(SIGINT, WebXClientConnector::signalHandler);
}

WebXClientConnector::~WebXClientConnector() {
    if (this->_publisher) {
        delete _publisher;
    }
    if (this->_collector) {
        delete _collector;
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
    WebXManager::instance()->getController()->addConnection(this->_publisher);

    this->_collector = new WebXClientCommandCollector();
}

void WebXClientConnector::run() {
    //  Prepare our context and socket
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_REP);
    int linger = 0;
    socket.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));

    char address[16];
    snprintf(address, sizeof(address) - 1, "tcp://*:%4d", WebXClientConnector::CONNECTOR_PORT);
    socket.bind(address);

    this->_publisher->run(this->_serializer, &context, WebXClientConnector::PUBLISHER_PORT);
    this->_collector->run(&context, WebXClientConnector::COLLECTOR_PORT);

    this->_running = true;
    while (this->_running) {
        zmq::message_t instructionMessage;

        //  Wait for next message from client
        try {
            socket.recv(&instructionMessage);

            // Deserialize instruction
            WebXInstruction instruction = this->_serializer->deserialize(instructionMessage.data(), instructionMessage.size());
            // printf("Got instruction %d \"%s\" %d\n", instruction.type, instruction.stringPayload.c_str(), instruction.integerPayload);

            // Handle message and get message
            WebXMessage * message = this->handleInstruction(instruction);

            // Send message
            if (message != NULL) {
                zmq::message_t * replyMessage = this->_serializer->serialize(message);
                socket.send(*replyMessage);

                delete message;
                delete replyMessage;

            } else {
                zmq::message_t replyMessage(0);
                socket.send(replyMessage);
            }
        
        } catch(zmq::error_t& e) {
            printf("ZeroMQ interrupted from message recv.\n");
        }
    }
}

void WebXClientConnector::stop() {
    WebXManager::instance()->getController()->removeConnection(this->_publisher);
    this->_running = false;
}

void WebXClientConnector::shutdown() {
    printf("Shutdown\n");

    printf("Stopping client connector...\n");
    _instance->stop();

    if (_instance) {
        delete _instance;
        _instance = NULL;
    }

    WebXManager::instance()->shutdown();
    printf("... client connector stopped\n");
}

WebXMessage * WebXClientConnector::handleInstruction(const WebXInstruction & instruction) {
    if (instruction.type == WebXInstruction::Type::Connect) {
        return this->handleConnectionInstruction();
    
    } else if (instruction.type == WebXInstruction::Type::Windows) {
        return this->handleWindowsInstruction();
    
    } else if (instruction.type == WebXInstruction::Type::Image) {
        return this->handleImageInstruction(instruction.numericPayload);
    }

    return NULL;
}

WebXMessage * WebXClientConnector::handleConnectionInstruction() {
    return new WebXConnectionMessage(WebXClientConnector::PUBLISHER_PORT, WebXClientConnector::COLLECTOR_PORT, this->_serializer->getType(), WebXManager::instance()->getDisplay()->getScreenSize());
}

WebXMessage * WebXClientConnector::handleWindowsInstruction() {
    return new WebXWindowsMessage(WebXManager::instance()->getController()->getWindows());
}

WebXMessage * WebXClientConnector::handleImageInstruction(long windowId) {
    std::shared_ptr<WebXImage> image = WebXManager::instance()->getDisplay()->getImage(windowId);

    return new WebXImageMessage(windowId, image);
}

