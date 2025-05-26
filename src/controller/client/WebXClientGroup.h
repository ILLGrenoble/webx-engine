#ifndef WEBX_CLIENT_GROUP_H
#define WEBX_CLIENT_GROUP_H

#include <vector>
#include <memory>
#include "WebXClient.h"
#include "WebXClientWindow.h"
#include <utils/WebXResult.h>
#include <models/WebXQuality.h>
#include <models/WebXSettings.h>
#include <models/WebXWindowDamage.h>
#include <models/WebXTransferData.h>
#include <utils/WebXOptional.h>

class WebXWindowVisibility;

/**
 * @class WebXClientGroup
 * @brief Represents a group of clients sharing the same quality level.
 * 
 * This class manages a collection of clients and their associated windows, handling
 * quality verification, window damage, and visibility updates.
 */
class WebXClientGroup {
public:
    /**
     * @brief Constructor for initializing a client group with specific quality settings.
     * @param settings The global settings for the group.
     * @param quality The quality level associated with the group.
     */
    WebXClientGroup(const WebXSettings & settings, const WebXQuality & quality);

    /**
     * @brief Destructor for cleaning up resources.
     */
    virtual ~WebXClientGroup();

    /**
     * @brief Gets the quality level associated with the group.
     * @return The quality level.
     */
    const WebXQuality & getQuality() const {
        return this->_quality;
    }

    /**
     * @brief Retrieves a client by its unique ID.
     * @param id The unique ID of the client.
     * @return A shared pointer to the client if found, otherwise nullptr.
     */
    std::shared_ptr<WebXClient> getClientById(uint32_t id) const {
        auto it = std::find_if(this->_clients.begin(), this->_clients.end(), [&id](const std::shared_ptr<WebXClient> & client) {
            return client->getId() == id;
        });

        return (it != this->_clients.end()) ? *it : nullptr;
    }

    /**
     * @brief Adds a client to the group if it does not already exist.
     * @param client A shared pointer to the client to be added.
     */
    void addClient(const std::shared_ptr<WebXClient> & client) {
        if (this->getClientById(client->getId()) == nullptr) {

            // Add client and update client index mask
            this->_clients.push_back(client);
            this->_clientIndexMask |= client->getIndex();

            // Reset bitrate data
            client->resetBitrateData(this->_averageImageMbps);
        }
    }

    /**
     * @brief Removes a client from the group by its unique ID.
     * @param clientId The unique ID of the client to be removed.
     */
    void removeClient(uint32_t clientId) {
        // Determine if client exists in vector and remove it
        const std::shared_ptr<WebXClient> & client = this->getClientById(clientId);
        if (client != nullptr) {
            auto it = std::find(this->_clients.begin(), this->_clients.end(), client);
            if (it != this->_clients.end()) {
                this->_clients.erase(it);
            }

            // Remove clientIndex bits from mask
            this->_clientIndexMask &= ~client->getIndex();
        }
    }

    /**
     * @brief Checks if the group has any clients.
     * @return True if the group has no clients, otherwise false.
     */
    bool hasClients() const {
        return this->_clients.empty();
    }

    /**
     * @brief Updates the visibility of windows based on the provided visibility data.
     * @param windowVisibilities A vector of visibility data for windows.
     */
    void updateVisibleWindows(const std::vector<const WebXWindowVisibility *> & windowVisibilities);

    /**
     * @brief Adds damage information to a window or creates a new window if it does not exist.
     * @param damage The damage information to be added.
     */
    void addWindowDamage(const WebXWindowDamage & damage) {
        // See if window exists or create new one
        auto it = std::find_if(this->_windows.begin(), this->_windows.end(), [&damage](const std::unique_ptr<WebXClientWindow> & window) {
            return window->getId() == damage.getX11Window();
        });
    
        // Modify or add window damage
        if (it == this->_windows.end()) {
            this->_windows.push_back(std::unique_ptr<WebXClientWindow>(new WebXClientWindow(damage.getX11Window(), this->_quality, damage, this->_settings.quality)));

        } else {
            std::unique_ptr<WebXClientWindow> & window = *it;
            window->addDamage(damage);
        }
    }

    /**
     * @brief Handles window graphical updates by invoking a provided handler function.
     * @param updateHandlerFunc A function to process window update and return transfer data.
     */
    void handleWindowGraphicalUpdates(std::function<WebXResult<WebXWindowImageTransferData>(const std::unique_ptr<WebXClientWindow> & window, uint64_t clientIndexMask)> updateHandlerFunc);
 
    /**
     * @brief Performs quality verification for all clients in the group.
     */
    void performQualityVerification() {
        // Update each client bitrate calculation
        for (auto client : this->_clients) {
            client->performQualityVerification(this->_averageImageMbps);
        }
    }

private:
    /**
     * @brief Calculates the average image Mbps based on transfer data points.
     */
    void calculateImageMbps();

private:
    const static int BITRATE_DATA_RETENTION_TIME_MS = 4000;
    const static int TIME_FOR_VALID_IMAGE_KBPS_MS = 2000;

    const WebXSettings & _settings;
    const WebXQuality & _quality;
    uint64_t _clientIndexMask;

    std::vector<std::shared_ptr<WebXClient>> _clients;
    std::vector<std::unique_ptr<WebXClientWindow>> _windows;

    std::vector<WebXTransferData> _transferDataPoints;
    WebXOptional<float> _averageImageMbps;
};


#endif /* WEBX_CLIENT_GROUP_H */