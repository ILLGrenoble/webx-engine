#ifndef WEBX_GATEWAY_H
#define WEBX_GATEWAY_H

#include <functional>
#include <memory>
#include <utils/WebXResult.h>

class WebXMessage;
class WebXInstruction;

class WebXGateway {

/**
 * Provides an interface between the transport layer (WebXClientInstructionSubscriber) and WebXController avoiding tight coupling between them.
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

    const WebXResult<std::pair<uint32_t, uint64_t>> onClientConnect() {
        if (this->_clientConnectFunc) {
            return this->_clientConnectFunc();
        }

        return WebXResult<std::pair<uint32_t, uint64_t>>::Err("engine configuration error");
    }

    const WebXResult<void> onClientDisconnect(uint32_t clientId) {
        if (this->_clientDisconnectFunc) {
            return this->_clientDisconnectFunc(clientId);
        }
        return WebXResult<void>::Err("engine configuration error");
    }

    void setMessagePublisherFunc(std::function<void(std::shared_ptr<WebXMessage>)> func) {
        this->_messagePublisherFunc = func;
    }

    void setInstructionHandlerFunc(std::function<void(std::shared_ptr<WebXInstruction>)> func) {
        this->_instructionHandlerFunc = func;
    }

    void setClientConnectFunc(std::function<const WebXResult<std::pair<uint32_t, uint64_t>>()> func) {
        this->_clientConnectFunc = func;
    }

    void setClientDisconnectFunc(std::function<const WebXResult<void>(uint32_t)> func) {
        this->_clientDisconnectFunc = func;
    }

private:
    std::function<void(std::shared_ptr<WebXMessage>)> _messagePublisherFunc;
    std::function<void(std::shared_ptr<WebXInstruction>)> _instructionHandlerFunc;
    std::function<const WebXResult<std::pair<uint32_t, uint64_t>>()> _clientConnectFunc;
    std::function<const WebXResult<void>(uint32_t)> _clientDisconnectFunc;
};


#endif /* WEBX_GATEWAY_H */