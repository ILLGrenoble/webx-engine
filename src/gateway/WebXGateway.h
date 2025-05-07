#ifndef WEBX_GATEWAY_H
#define WEBX_GATEWAY_H

#include <functional>
#include <memory>
#include <utils/WebXResult.h>
#include <models/WebXVersion.h>

class WebXMessage;
class WebXInstruction;

/**
 * @class WebXGateway
 * @brief Provides an interface between the transport layer (WebXClientInstructionSubscriber) and WebXController.
 * 
 * The WebXGateway class acts as a mediator to avoid tight coupling between the transport layer and the controller.
 * It facilitates message and instruction passing between these components while avoiding tight coupling between them.
 */
class WebXGateway {
public:
    /**
     * @brief Default constructor initializing function pointers to nullptr.
     */
    WebXGateway() : 
        _messagePublisherFunc(nullptr),
        _instructionHandlerFunc(nullptr) 
        {}

    /**
     * @brief Virtual destructor.
     */
    virtual ~WebXGateway() {}

    /**
     * @brief Publishes a message using the provided message publisher function.
     * @param message A shared pointer to the WebXMessage to be published.
     */
    void publishMessage(std::shared_ptr<WebXMessage> message) {
        if (this->_messagePublisherFunc) {
            this->_messagePublisherFunc(message);
        }
    }

    /**
     * @brief Handles an instruction using the provided instruction handler function.
     * @param instruction A shared pointer to the WebXInstruction to be handled.
     */
    void handleInstruction(std::shared_ptr<WebXInstruction> instruction) {
        if (this->_instructionHandlerFunc) {
            this->_instructionHandlerFunc(instruction);
        }
    }

    /**
     * @brief Handles client connection events.
     * @param clientVersion The version of the client connecting.
     * @return A WebXResult containing a pair of client ID and timestamp, or an error message.
     */
    const WebXResult<std::pair<uint32_t, uint64_t>> onClientConnect(const WebXVersion & clientVersion) {
        if (this->_clientConnectFunc) {
            return this->_clientConnectFunc(clientVersion);
        }

        return WebXResult<std::pair<uint32_t, uint64_t>>::Err("engine configuration error");
    }

    /**
     * @brief Handles client disconnection events.
     * @param clientId The ID of the client to be disconnected.
     * @return A WebXResult indicating success or an error message.
     */
    const WebXResult<void> onClientDisconnect(uint32_t clientId) {
        if (this->_clientDisconnectFunc) {
            return this->_clientDisconnectFunc(clientId);
        }
        return WebXResult<void>::Err("engine configuration error");
    }

    /**
     * @brief Sets the function to publish messages.
     * @param func A function that takes a shared pointer to WebXMessage.
     */
    void setMessagePublisherFunc(std::function<void(std::shared_ptr<WebXMessage>)> func) {
        this->_messagePublisherFunc = func;
    }

    /**
     * @brief Sets the function to handle instructions.
     * @param func A function that takes a shared pointer to WebXInstruction.
     */
    void setInstructionHandlerFunc(std::function<void(std::shared_ptr<WebXInstruction>)> func) {
        this->_instructionHandlerFunc = func;
    }

    /**
     * @brief Sets the function to handle client connections.
     * @param func A function that returns a WebXResult containing a pair of client ID and timestamp.
     */
    void setClientConnectFunc(std::function<const WebXResult<std::pair<uint32_t, uint64_t>>(const WebXVersion & clientVersion)> func) {
        this->_clientConnectFunc = func;
    }

    /**
     * @brief Sets the function to handle client disconnections.
     * @param func A function that takes a client ID and returns a WebXResult.
     */
    void setClientDisconnectFunc(std::function<const WebXResult<void>(uint32_t)> func) {
        this->_clientDisconnectFunc = func;
    }

private:
    /**
     * @brief Function to publish messages.
     */
    std::function<void(std::shared_ptr<WebXMessage>)> _messagePublisherFunc;

    /**
     * @brief Function to handle instructions.
     */
    std::function<void(std::shared_ptr<WebXInstruction>)> _instructionHandlerFunc;

    /**
     * @brief Function to handle client connections.
     */
    std::function<const WebXResult<std::pair<uint32_t, uint64_t>>(const WebXVersion & clientVersion)> _clientConnectFunc;

    /**
     * @brief Function to handle client disconnections.
     */
    std::function<const WebXResult<void>(uint32_t)> _clientDisconnectFunc;
};

#endif /* WEBX_GATEWAY_H */