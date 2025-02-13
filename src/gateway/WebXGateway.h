#ifndef WEBX_GATEWAY_H
#define WEBX_GATEWAY_H

#include <functional>
#include <memory>

class WebXMessage;
class WebXInstruction;

class WebXGateway {

/**
 * Provides an interface between the transport layer (WebXClientCommandCollector) and WebXController avoiding tight coupling between them.
 * The WebXGateway is visible to both transport and controller and allows for message/instruction passing between them.
 */
public:
    WebXGateway() : 
        _messagePublisherFunc(nullptr),
        _instructionHandlerFunc(nullptr) 
        {}
    virtual ~WebXGateway() {}

    void publishMessage(std::shared_ptr<WebXMessage> message) {
        if (this->_messagePublisherFunc) {
            this->_messagePublisherFunc(message);
        }
    }

    void handleInstruction(std::shared_ptr<WebXInstruction> instruction) {
        if (this->_instructionHandlerFunc) {
            this->_instructionHandlerFunc(instruction);
        }
    }

    void setMessagePublisherFunc(std::function<void(std::shared_ptr<WebXMessage>)> func) {
        this->_messagePublisherFunc = func;
    }

    void setInstructionHandlerFunc(std::function<void(std::shared_ptr<WebXInstruction>)> func) {
        this->_instructionHandlerFunc = func;
    }

private:
    std::function<void(std::shared_ptr<WebXMessage>)> _messagePublisherFunc;
    std::function<void(std::shared_ptr<WebXInstruction>)> _instructionHandlerFunc;
};


#endif /* WEBX_GATEWAY_H */