#include "WebXClientConnector.h"
#include "message/WebXConnectionMessage.h"
#include "message/WebXScreenMessage.h"
#include "message/WebXWindowsMessage.h"
#include "message/WebXImageMessage.h"
#include "instruction/WebXImageInstruction.h"
#include "instruction/WebXMouseInstruction.h"
#include "serializer/WebXJsonSerializer.h"
#include "serializer/WebXBinarySerializer.h"
#include "WebXClientMessagePublisher.h"
#include "WebXClientCommandCollector.h"
#include <display/WebXManager.h>
#include <display/WebXDisplay.h>
#include <display/WebXController.h>
#include <utils/WebXBinaryBuffer.h>
#include <unistd.h>
#include <string>
#include <zmq.hpp>
#include <spdlog/spdlog.h>
#include <X11/Xlib.h>

WebXClientConnector * WebXClientConnector::_instance = NULL;
int WebXClientConnector::CONNECTOR_PORT = 5555;
int WebXClientConnector::COLLECTOR_PORT = 5556;
int WebXClientConnector::PUBLISHER_PORT = 5557;

WebXClientConnector::WebXClientConnector(std::string &transport) :
    _publisher(NULL),
    _collector(NULL),
    _serializer(NULL),
    _running(false) {
    spdlog::info("Using {} for the transport", transport);
    if(transport == "json") {
        _serializer = new WebXJsonSerializer();
    } else {
        _serializer = new WebXBinarySerializer();
    }
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

WebXClientConnector * WebXClientConnector::initInstance(std::string &transport) {
    if (_instance == NULL) {
        _instance = new WebXClientConnector(transport);
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
    this->_collector->run(this->_serializer, &context, WebXClientConnector::COLLECTOR_PORT);

    this->_running = true;
    while (this->_running) {
        zmq::message_t instructionMessage;

        //  Wait for next message from client
        try {
            socket.recv(&instructionMessage);
            const char * instructionData = (const char *)instructionMessage.data();
            if (instructionMessage.size() == 4 && strncmp(instructionData, "comm", 4) == 0) {
                const std::string & serializerType = this->_serializer->getType();
                zmq::message_t replyMessage(serializerType.c_str(), serializerType.size());
                socket.send(replyMessage);

            } else {
                // Deserialize instruction
                WebXInstruction * instruction = this->_serializer->deserialize(instructionMessage.data(), instructionMessage.size());
                if(instruction != NULL) {
                    // Handle message and get message
                    WebXMessage *message = this->handleInstruction(instruction);
                    delete instruction;
                    // Send message
                    if (message != NULL) {
                        message->commandId = instruction->id;
                        zmq::message_t *replyMessage = this->_serializer->serialize(message);
                        socket.send(*replyMessage);

                        delete message;
                        delete replyMessage;

                    } else {
                        zmq::message_t replyMessage(0);
                        socket.send(replyMessage);
                    }
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
    }

    // Suicide
    delete this;

    spdlog::info("Stopped client connector");
}

void WebXClientConnector::stop() {
    spdlog::info("Stopping client connector...");
    WebXManager::instance()->getController()->removeConnection(this->_publisher);

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

WebXMessage * WebXClientConnector::handleInstruction(WebXInstruction * instruction) {
    if (instruction->type == WebXInstruction::Type::Connect) {
        return this->handleConnectionInstruction();
    
    } else if (instruction->type == WebXInstruction::Type::Screen) {
        return this->handleScreenInstruction();
    
    } else if (instruction->type == WebXInstruction::Type::Windows) {
        return this->handleWindowsInstruction();
    
    } else if (instruction->type == WebXInstruction::Type::Image) {
        WebXImageInstruction * imageInstruction = (WebXImageInstruction *)instruction;
        return this->handleImageInstruction(imageInstruction->windowId);
    }

    return NULL;
}



WebXMessage * WebXClientConnector::handleConnectionInstruction() {
    return new WebXConnectionMessage(WebXClientConnector::PUBLISHER_PORT, WebXClientConnector::COLLECTOR_PORT);
}

WebXMessage * WebXClientConnector::handleScreenInstruction() {
    return new WebXScreenMessage(WebXManager::instance()->getDisplay()->getScreenSize());
}

WebXMessage * WebXClientConnector::handleWindowsInstruction() {
    return new WebXWindowsMessage(WebXManager::instance()->getController()->getWindows());
}

WebXMessage * WebXClientConnector::handleImageInstruction(long windowId) {
    std::shared_ptr<WebXImage> image = WebXManager::instance()->getDisplay()->getImage(windowId);

    return new WebXImageMessage(windowId, image);
}

