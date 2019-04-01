#ifndef WEBX_ENTRY_H
#define WEBX_ENTRY_H

#include <csignal>
#include "WebXClientRequest.h"

class WebXManager;
class WebXClientPublisher;
class WebXClientCollector;
class WebXClientConnectorResponse;

class WebXClientConnector {
private:
    WebXClientConnector();
    virtual ~WebXClientConnector();
    void init();

public:
    static WebXClientConnector * initInstance();
    static WebXClientConnector * instance() {
        return _instance;
    }
    static void shutdown();

    void run();
    void stop();

    static void signalHandler(int signal) {
        if (signal == SIGINT) {
            shutdown();
        }
    }

private:
    WebXClientConnectorResponse * handleRequest(const WebXClientRequest & request);
    WebXClientConnectorResponse * handleConnectionRequest();
    WebXClientConnectorResponse * handleWindowsRequest();


private:
    static WebXClientConnector * _instance;
    static int CONNECTOR_PORT;
    static int COLLECTOR_PORT;
    static int PUBLISHER_PORT;

    WebXClientPublisher * _publisher;
    WebXClientCollector * _collector;

    bool _running;
};


#endif /* WEBX_ENTRY_H */