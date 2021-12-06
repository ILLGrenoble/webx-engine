#include "WebXTransport.h"
#include "WebXClientConnector.h"
#include "WebXClientMessagePublisher.h"
#include "WebXClientCommandCollector.h"
#include <controller/WebXController.h>
#include <serializer/WebXBinarySerializer.h>


int WebXTransport::CONNECTOR_PORT = 5558;
int WebXTransport::COLLECTOR_PORT = 5556;
int WebXTransport::PUBLISHER_PORT = 5557;

std::string WebXTransport::EVENT_BUS_ADDR = "inproc://webx-engine/event-bus";

WebXTransport::WebXTransport(bool standAlone) :
    _standAlone(standAlone),
    _serializer(new WebXBinarySerializer()),
    _context(1),
    _connector(new WebXClientConnector()),
    _publisher(new WebXClientMessagePublisher()),
    _collector(new WebXClientCommandCollector()) {
}

WebXTransport::~WebXTransport() {
    delete _connector;
    delete _publisher;
    delete _collector;
    delete _serializer;
}

void WebXTransport::start() {

    // Create event bus publisher
    this->_eventBusPublisher = this->createEventBusPublisher();

    if (this->_standAlone) {
        // Create connector
        const std::string clientResponderAddr = fmt::format("tcp://*:{:4d}", CONNECTOR_PORT);
        _connector->run(this->_serializer, &this->_context, clientResponderAddr, EVENT_BUS_ADDR, PUBLISHER_PORT, COLLECTOR_PORT);

        // Create publisher
        std::string clientPublisherAddr = fmt::format("tcp://*:{:4d}", PUBLISHER_PORT);
        _publisher->run(this->_serializer, &this->_context, clientPublisherAddr, true, EVENT_BUS_ADDR);

        // Create instruction collector
        const std::string clientCollectorAddr = fmt::format("tcp://*:{:4d}", COLLECTOR_PORT);
        _collector->run(this->_serializer, &this->_context, clientCollectorAddr, true, EVENT_BUS_ADDR);
    
    } else {
        // Create publisher
        std::string clientPublisherAddr = "ipc:///tmp/webx-router-message-proxy.ipc";
        _publisher->run(this->_serializer, &this->_context, clientPublisherAddr, false, EVENT_BUS_ADDR);

        // Create instruction collector
        std::string clientCollectorAddr = "ipc:///tmp/webx-router-instruction-proxy.ipc";
        _collector->run(this->_serializer, &this->_context, clientCollectorAddr, false, EVENT_BUS_ADDR);
    }

    WebXController::instance()->setConnection(this->_publisher);
}

void WebXTransport::stop() {
    WebXController::instance()->removeConnection();

    // Send shutdown message
    std::string messageString = "shutdown";
    zmq::message_t message(messageString.c_str(), messageString.size());
    this->_eventBusPublisher.send(message, zmq::send_flags::none);

    _connector->stop();
    _publisher->stop();
    _collector->stop();
}

zmq::socket_t WebXTransport::createEventBusPublisher() {
    zmq::socket_t socket(this->_context, ZMQ_PUB);
    int linger = 0;
    socket.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
    try {
        socket.bind(EVENT_BUS_ADDR);
        return socket;
    
    } catch (zmq::error_t& e) {
        spdlog::error("failed to bind Event Bus socket to {:s}", EVENT_BUS_ADDR);
        exit(1);
    }       
}
