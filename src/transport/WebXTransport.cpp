#include "WebXTransport.h"
#include "WebXClientConnector.h"
#include "WebXClientMessagePublisher.h"
#include "WebXClientCommandCollector.h"
#include <controller/WebXController.h>
#include <serializer/WebXBinarySerializer.h>
#include <utils/WebXSettings.h>

WebXTransport::WebXTransport(WebXSettings * settings, bool standAlone) :
    _settings(settings),
    _standAlone(standAlone),
    _serializer(new WebXBinarySerializer(settings)),
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
        const std::string clientResponderAddr = fmt::format("tcp://*:{:4d}", _settings->connectorPort);
        _connector->run(this->_serializer, &this->_context, clientResponderAddr, _settings->inprocEventBusAddress, _settings);

        // Create publisher
        std::string clientPublisherAddr = fmt::format("tcp://*:{:4d}", _settings->publisherPort);
        _publisher->run(this->_serializer, &this->_context, clientPublisherAddr, true, _settings->inprocEventBusAddress);

        // Create instruction collector
        const std::string clientCollectorAddr = fmt::format("tcp://*:{:4d}", _settings->collectorPort);
        _collector->run(this->_serializer, &this->_context, clientCollectorAddr, true, _settings->inprocEventBusAddress, _settings);
    
    } else {
        // Create connector
        std::string sessionConnectorAddr = fmt::format("ipc://{}", _settings->ipcSessionConnectorPath);
        _connector->run(this->_serializer, &this->_context, sessionConnectorAddr, _settings->inprocEventBusAddress, _settings);

        // Create publisher
        std::string clientPublisherAddr = fmt::format("ipc://{}", _settings->ipcMessageProxyPath);
        _publisher->run(this->_serializer, &this->_context, clientPublisherAddr, false, _settings->inprocEventBusAddress);

        // Create instruction collector
        std::string clientCollectorAddr = fmt::format("ipc://{}", _settings->ipcInstructionProxyPath);
        _collector->run(this->_serializer, &this->_context, clientCollectorAddr, false, _settings->inprocEventBusAddress, _settings);
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
        socket.bind(_settings->inprocEventBusAddress);
        return socket;
    
    } catch (zmq::error_t& e) {
        spdlog::error("failed to bind Event Bus socket to {:s}", _settings->inprocEventBusAddress);
        exit(1);
    }       
}
