#ifndef WEBX_CLIENT_REGISTRY_H
#define WEBX_CLIENT_REGISTRY_H

#include <vector>
#include <memory>
#include <random>

#include "WebXClient.h"
#include "WebXClientGroup.h"
#include <utils/WebXResult.h>
#include <models/WebXQuality.h>
#include <models/WebXSettings.h>

class WebXMessage;

/**
 * @class WebXClientRegistry
 * @brief Manages the registry of clients and their associated groups.
 * 
 * This class is responsible for adding, removing, and managing clients, as well as
 * handling client-related events such as pings, data acknowledgments, and window damage.
 */
class WebXClientRegistry {
public:
    /**
     * @brief Constructor for initializing the client registry.
     * @param settings The global settings for the registry.
     * @param clientMessageHandler The handler function for client messages.
     */
    WebXClientRegistry(const WebXSettings & settings, const std::function<void(std::shared_ptr<WebXMessage> clientMessage)> clientMessageHandler);

    /**
     * @brief Destructor for cleaning up resources.
     */
    virtual ~WebXClientRegistry();

    /**
     * @brief Adds a new client to the registry.
     * @param clientVersion The version of the client.
     * @return A result containing the client ID and index if successful.
     */
    const WebXResult<std::pair<uint32_t, uint64_t>> addClient(const WebXVersion & clientVersion);

    /**
     * @brief Removes a client from the registry.
     * @param clientId The ID of the client to be removed.
     * @return A result indicating success or failure.
     */
    const WebXResult<void> removeClient(uint32_t clientId);
    
    /**
     * @brief Checks if a client ID is valid.
     * @param clientId The ID of the client to check.
     * @return True if the client ID is valid, false otherwise.
     */
    const bool isValidClient(uint32_t clientId) const {
        return this->getClientById(clientId) != nullptr;
    }

    /**
     * @brief Retrieves a client by its ID.
     * @param id The ID of the client to retrieve.
     * @return A shared pointer to the client if found, nullptr otherwise.
     */
    std::shared_ptr<WebXClient> getClientById(uint32_t id) const {
        const std::lock_guard<std::recursive_mutex> lock(this->_mutex);
        auto it = std::find_if(this->_clients.begin(), this->_clients.end(), [&id](const std::shared_ptr<WebXClient> & client) {
            return client->getId() == id;
        });

        return (it != this->_clients.end()) ? *it : nullptr;
    }

    /**
     * @brief Sets the maximum quality for a client.
     * @param clientId The ID of the client.
     * @param quality The maximum quality to set.
     */
    void setClientMaxQuality(uint32_t clientId, const WebXQuality & quality);

    /**
     * @brief Handles client pings.
     */
    void handleClientPings();

    /**
     * @brief Handles the reception of a pong message from a client. This is used to calculate a client's Round-Trip Time latency.
     * @param clientId The ID of the client.
     * @param sendTimestampMs The timestamp when the pong was sent.
     * @param recvTimestampMs The timestamp when the pong was received.
     */
    void onPongReceived(uint32_t clientId, uint64_t sendTimestampMs, uint64_t recvTimestampMs) {
        const std::lock_guard<std::recursive_mutex> lock(this->_mutex);
        // Get associated client
        std::shared_ptr<WebXClient> client = this->getClientById(clientId);
        if (client != nullptr) {
            spdlog::trace("Received Pong for client with Id {:08x} and index {:016x}", client->getId(), client->getIndex());
            client->onPongReceived(sendTimestampMs, recvTimestampMs);
        }
    }
    
    /**
     * @brief Handles the reception of a data acknowledgment from a client. This is used to calculate a client's bandwidth.
     * @param clientId The ID of the client.
     * @param sendTimestampMs The timestamp when the data was sent.
     * @param recvTimestampMs The timestamp when the acknowledgment was received.
     * @param dataLength The length of the acknowledged data.
     */
    void onDataAckReceived(uint32_t clientId, uint64_t sendTimestampMs, uint64_t recvTimestampMs, uint32_t dataLength) {
        const std::lock_guard<std::recursive_mutex> lock(this->_mutex);
        // Get associated client
        std::shared_ptr<WebXClient> client = this->getClientById(clientId);
        if (client != nullptr) {
            client->onDataAckReceived(sendTimestampMs, recvTimestampMs, dataLength);
        }
    }

    /**
     * @brief Adds window damage information to all client groups.
     * @param damage The window damage information.
     */
    void addWindowDamage(const WebXWindowDamage & damage) {
        const std::lock_guard<std::recursive_mutex> lock(this->_mutex);
        for (auto & group : this->_groups) {
            group->addWindowDamage(damage);
        }
    }

    /**
     * @brief Updates the visibility of windows for all client groups.
     * @param windowVisibilities A vector of window visibility information.
     */
    void updateVisibleWindows(const std::vector<const WebXWindowVisibility *> & windowVisibilities) {
        const std::lock_guard<std::recursive_mutex> lock(this->_mutex);
        for (auto & group : this->_groups) {
            group->updateVisibleWindows(windowVisibilities);
        }
    }

    /**
     * @brief Handles window damage for all client groups.
     * @param damageHandlerFunc The function to handle window damage.
     */
    void handleWindowDamage(std::function<WebXResult<WebXWindowImageTransferData>(const std::unique_ptr<WebXClientWindow> & window, uint64_t clientIndexMask)> damageHandlerFunc) {
        const std::lock_guard<std::recursive_mutex> lock(this->_mutex);
        for (auto & group : this->_groups) {
            group->handleWindowDamage(damageHandlerFunc);
        }
    }

    /**
     * @brief Performs quality verification for all clients.
     */
    void performQualityVerification();

    /**
     * Disconnects all clients
     */
    void disconnectAll();

private:
    /**
     * @brief Retrieves a client group by its quality.
     * @param quality The quality of the client group.
     * @return A shared pointer to the client group if found, nullptr otherwise.
     */
    std::shared_ptr<WebXClientGroup> getGroupByQuality(const WebXQuality & quality) const {
        auto it = std::find_if(this->_groups.begin(), this->_groups.end(), [&quality](const std::shared_ptr<WebXClientGroup> & group) {
            return group->getQuality() == quality;
        });

        return (it != this->_groups.end()) ? *it : nullptr;
    }

    /**
     * @brief Retrieves or creates a client group by its quality.
     * @param quality The quality of the client group.
     * @return A shared pointer to the client group.
     */
    std::shared_ptr<WebXClientGroup> getOrCreateGroupByQuality(const WebXQuality & quality) {
        auto it = std::find_if(this->_groups.begin(), this->_groups.end(), [&quality](const std::shared_ptr<WebXClientGroup> & group) {
            return group->getQuality() == quality;
        });

        if (it == this->_groups.end()) {
            auto group = std::make_shared<WebXClientGroup>(this->_settings, quality);
            this->_groups.push_back(group);
            spdlog::trace("Created group with with quality index {:d}. Now have {:d} client groups", group->getQuality().index, this->_groups.size());
            return group;

        } else {
            return *it;
        }
    }

    /**
     * @brief Retrieves a client group that contains a specific client ID.
     * @param clientId The ID of the client.
     * @return A shared pointer to the client group if found, nullptr otherwise.
     */
    std::shared_ptr<WebXClientGroup> getGroupWithClientId(uint32_t clientId) const {
        auto it = std::find_if(this->_groups.begin(), this->_groups.end(), [&clientId](const std::shared_ptr<WebXClientGroup> & group) {
            return group->getClientById(clientId) != nullptr;
        });

        return (it != this->_groups.end()) ? *it : nullptr;
    }

    /**
     * @brief Sets the quality for a client.
     * @param client The client to set the quality for.
     * @param quality The quality to set.
     */
    void setClientQuality(std::shared_ptr<WebXClient> client, const WebXQuality & quality);

    /**
     * @brief Removes a client from all groups.
     * @param clientId The ID of the client to remove.
     */
    void removeClientFromGroups(uint32_t clientId);

    /**
     * @brief Removes a client from a specific group.
     * @param clientId The ID of the client to remove.
     * @param group The group to remove the client from.
     */
    void removeClientFromGroup(uint32_t clientId, const std::shared_ptr<WebXClientGroup> & group);

private:
    const WebXSettings & _settings;
    const std::function<void(std::shared_ptr<WebXMessage> clientMessage)> _clientMessageHandler;
    std::mt19937 _randomNumberGenerator;
    uint64_t _clientIndexMask;

    mutable std::recursive_mutex _mutex;

    std::vector<std::shared_ptr<WebXClient>> _clients;
    std::vector<std::shared_ptr<WebXClientGroup>> _groups;
};


#endif /* WEBX_CLIENT_REGISTRY_H */