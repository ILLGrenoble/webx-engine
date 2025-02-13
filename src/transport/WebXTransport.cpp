#include "WebXTransport.h"
#include "WebXClientConnector.h"
#include "WebXClientMessagePublisher.h"
#include "WebXClientCommandCollector.h"
#include <gateway/WebXGateway.h>
#include <serializer/WebXBinarySerializer.h>
#include <utils/WebXSettings.h>

WebXTransport::WebXTransport(WebXGateway * gateway, WebXSettings * settings, bool standAlone) :
    _gateway(gateway),
    _settings(settings),
    _standAlone(standAlone),
    _serializer(new WebXBinarySerializer(settings)),
    _context(1),
    _eventBusPublisher(this->createEventBusPublisher()),
    _connector(new WebXClientConnector(_serializer)),
    _publisher(new WebXClientMessagePublisher(_serializer)),
    _collector(new WebXClientCommandCollector(_gateway, _serializer)) {
}

WebXTransport::~WebXTransport() {
    delete _connector;
    delete _publisher;
    delete _collector;
    delete _serializer;
}

void WebXTransport::start() {
    if (this->_standAlone) {
        // Create connector
        const std::string clientResponderAddr = fmt::format("tcp://*:{:4d}", _settings->connectorPort);
        _connector->run(&this->_context, clientResponderAddr, _settings->inprocEventBusAddress, _settings);

        // Create publisher
        std::string clientPublisherAddr = fmt::format("tcp://*:{:4d}", _settings->publisherPort);
        _publisher->run(&this->_context, clientPublisherAddr, true, _settings->inprocEventBusAddress);

        // Create instruction collector
        const std::string clientCollectorAddr = fmt::format("tcp://*:{:4d}", _settings->collectorPort);
        _collector->run(&this->_context, clientCollectorAddr, true, _settings->inprocEventBusAddress, _settings);
    
    } else {
        // Create connector
        std::string sessionConnectorAddr = fmt::format("ipc://{}", _settings->ipcSessionConnectorPath);
        _connector->run(&this->_context, sessionConnectorAddr, _settings->inprocEventBusAddress, _settings);

        // Create publisher
        std::string clientPublisherAddr = fmt::format("ipc://{}", _settings->ipcMessageProxyPath);
        _publisher->run(&this->_context, clientPublisherAddr, false, _settings->inprocEventBusAddress);

        // Create instruction collector
        std::string clientCollectorAddr = fmt::format("ipc://{}", _settings->ipcInstructionProxyPath);
        _collector->run(&this->_context, clientCollectorAddr, false, _settings->inprocEventBusAddress, _settings);
    }

    // Set the message publisher function in the gateway
    this->_gateway->setMessagePublisherFunc([this](std::shared_ptr<WebXMessage> message) {
        this->_publisher->onMessage(message);
    });
}

void WebXTransport::stop() {
    // Remove the message publisher function in the gateway
    this->_gateway->setMessagePublisherFunc(nullptr);

    // Send shutdown message
    std::string messageString = "shutdown";
    zmq::message_t message(messageString.c_str(), messageString.size());
#ifdef COMPILE_FOR_CPPZMQ_BEFORE_4_3_1
    this->_eventBusPublisher.send(message);
#else
    this->_eventBusPublisher.send(message, zmq::send_flags::none);
#endif

    _connector->stop();
    _publisher->stop();
    _collector->stop();
}

zmq::socket_t WebXTransport::createEventBusPublisher() {
    zmq::socket_t socket(this->_context, ZMQ_PUB);
#ifdef COMPILE_FOR_CPPZMQ_BEFORE_4_8_0
    int linger = 0;
    socket.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));
#else
    socket.set(zmq::sockopt::linger, 0);
#endif
    try {
        socket.bind(_settings->inprocEventBusAddress);
        return socket;
    
    } catch (zmq::error_t& e) {
        spdlog::error("failed to bind Event Bus socket to {:s}", _settings->inprocEventBusAddress);
        exit(1);
    }       
}
