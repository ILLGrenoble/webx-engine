#ifndef WEBX_ENTRY_H
#define WEBX_ENTRY_H

#include <csignal>
#include <memory>
#include "instruction/WebXInstruction.h"
#include "message/WebXMessage.h"

class WebXManager;
class WebXClientMessagePublisher;
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

    void run(int socketTimoutMs);
    void stop();

    static void signalHandler(int signal) {
        if (signal == SIGINT) {
            shutdown();
        }
    }

private:
    std::shared_ptr<WebXMessage> handleInstruction(std::shared_ptr<WebXInstruction> instruction);
    std::shared_ptr<WebXMessage> handleConnectionInstruction();
    std::shared_ptr<WebXMessage> handleScreenInstruction();
    std::shared_ptr<WebXMessage> handleWindowsInstruction();
    std::shared_ptr<WebXMessage> handleImageInstruction(long windowId);
    std::shared_ptr<WebXMessage> handleCursorInstruction(uint32_t cursorId);

private:
    static WebXClientConnector * _instance;
    static int CONNECTOR_PORT;
    static int PUBLISHER_PORT;

    WebXClientMessagePublisher * _publisher;

    WebXSerializer * _serializer;
    bool _running;
};


#endif /* WEBX_ENTRY_H */