#include "WebXClientConnector.h"
#include "response/WebXConnectionResponse.h"
#include "response/WebXWindowsResponse.h"
#include "response/WebXImageResponse.h"
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

    this->_publisher->run(&context, WebXClientConnector::PUBLISHER_PORT);
    this->_collector->run(&context, WebXClientConnector::COLLECTOR_PORT);

    this->_running = true;
    while (this->_running) {
        zmq::message_t requestMessage;

        //  Wait for next message from client
        try {
            socket.recv(&requestMessage);

            // Handle message
            std::string requestData = std::string(static_cast<char*>(requestMessage.data()), requestMessage.size());
            printf("request: %s\n", requestData.c_str());

            // Convert to json
            nlohmann::json jRequest = nlohmann::json::parse(requestData);

            // Convert to request
            WebXClientRequest request = jRequest.get<WebXClientRequest>();
            // printf("Got request %d \"%s\" %d\n", request.type, request.stringPayload.c_str(), request.integerPayload);

            // Handle message and get response
            WebXResponse * response = this->handleRequest(request);

            // Send response
            if (response != NULL) {
                nlohmann::json jResponse;
                response->toJson(jResponse);
                std::string responseData = jResponse.dump();
                // printf("response: %s\n", responseData.c_str());

                zmq::message_t replyMessage(responseData.size());
                memcpy(replyMessage.data(), responseData.c_str(), responseData.size());
                socket.send(replyMessage);

                delete response;

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

WebXResponse * WebXClientConnector::handleRequest(const WebXClientRequest & request) {
    if (request.type == WebXClientRequest::Type::Connect) {
        return this->handleConnectionRequest();
    
    } else if (request.type == WebXClientRequest::Type::Windows) {
        return this->handleWindowsRequest();
    
    } else if (request.type == WebXClientRequest::Type::Image) {
        return this->handleImageRequest(request.numericPayload);
    }

    return NULL;
}

WebXResponse * WebXClientConnector::handleConnectionRequest() {
    return new WebXConnectionResponse(WebXClientConnector::PUBLISHER_PORT, WebXClientConnector::COLLECTOR_PORT, WebXManager::instance()->getDisplay()->getScreenSize());
}

WebXResponse * WebXClientConnector::handleWindowsRequest() {
    return new WebXWindowsResponse(WebXManager::instance()->getController()->getWindows());
}

WebXResponse * WebXClientConnector::handleImageRequest(long windowId) {
    std::shared_ptr<WebXImage> image = WebXManager::instance()->getDisplay()->getImage(windowId);

    return new WebXImageResponse(windowId, image);
}

