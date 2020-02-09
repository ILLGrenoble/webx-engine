#ifndef WEBX_ENTRY_H
#define WEBX_ENTRY_H

#include <csignal>
#include <memory>
#include "instruction/WebXInstruction.h"

class WebXManager;
class WebXClientMessagePublisher;
class WebXClientCommandCollector;
class WebXMessage;
class WebXSerializer;

class WebXClientConnector {
private:
    WebXClientConnector(std::string &transport);
    virtual ~WebXClientConnector();
    void init();

public:
    static WebXClientConnector * initInstance(std::string &transport);
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
    WebXMessage * handleInstruction(std::shared_ptr<WebXInstruction> instruction);
    WebXMessage * handleConnectionInstruction();
    WebXMessage * handleScreenInstruction();
    WebXMessage * handleWindowsInstruction();
    WebXMessage * handleImageInstruction(long windowId);
    WebXMessage * handleCursorInstruction(uint32_t cursorId);

private:
    static WebXClientConnector * _instance;
    static int CONNECTOR_PORT;
    static int COLLECTOR_PORT;
    static int PUBLISHER_PORT;

    WebXClientMessagePublisher * _publisher;
    WebXClientCommandCollector * _collector;

    WebXSerializer * _serializer;
    bool _running;
};


#endif /* WEBX_ENTRY_H */