#ifndef WEBX_ENTRY_H
#define WEBX_ENTRY_H

#include <csignal>
#include "request/WebXClientRequest.h"

class WebXManager;
class WebXClientMessagePublisher;
class WebXClientCommandCollector;
class WebXResponse;

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
    WebXResponse * handleRequest(const WebXClientRequest & request);
    WebXResponse * handleConnectionRequest();
    WebXResponse * handleWindowsRequest();
    WebXResponse * handleImageRequest(long windowId);


private:
    static WebXClientConnector * _instance;
    static int CONNECTOR_PORT;
    static int COLLECTOR_PORT;
    static int PUBLISHER_PORT;

    WebXClientMessagePublisher * _publisher;
    WebXClientCommandCollector * _collector;

    bool _running;
};


#endif /* WEBX_ENTRY_H */