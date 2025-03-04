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

class WebXClientRegistry {

public:
    WebXClientRegistry(const WebXSettings & settings);
    virtual ~WebXClientRegistry();

    const WebXResult<std::pair<uint32_t, uint64_t>> addClient();
    const WebXResult<void> removeClient(uint32_t clientId);
    
    const bool isValidClient(uint32_t clientId) const {
        return this->getClientById(clientId) != nullptr;
    }

    std::shared_ptr<WebXClient> getClientById(uint32_t id) const {
        const std::lock_guard<std::recursive_mutex> lock(this->_mutex);
        auto it = std::find_if(this->_clients.begin(), this->_clients.end(), [&id](const std::shared_ptr<WebXClient> & client) {
            return client->getId() == id;
        });

        return (it != this->_clients.end()) ? *it : nullptr;
    }

    void setClientQuality(uint32_t clientId, const WebXQuality & quality);

    void handleClientPings(const std::function<void(uint64_t clientIndex)> clientPingHandler);

    void onPongReceived(uint32_t clientId);

    void addWindowDamage(const WebXWindowDamage & damage) {
        const std::lock_guard<std::recursive_mutex> lock(this->_mutex);
        for (auto & group : this->_groups) {
            group->addWindowDamage(damage);
        }
    }

    void updateVisibleWindows(const std::vector<const WebXWindowVisibility *> & windowVisibilities) {
        const std::lock_guard<std::recursive_mutex> lock(this->_mutex);
        for (auto & group : this->_groups) {
            group->updateVisibleWindows(windowVisibilities);
        }
    }

    void handleWindowDamage(std::function<WebXResult<WebXWindowImageTransferData>(const std::unique_ptr<WebXClientWindow> & window, uint64_t clientIndexMask)> damageHandlerFunc) {
        const std::lock_guard<std::recursive_mutex> lock(this->_mutex);
        for (auto & group : this->_groups) {
            group->handleWindowDamage(damageHandlerFunc);
        }
    }

private:
    std::shared_ptr<WebXClientGroup> getGroupByQuality(const WebXQuality & quality) const {
        auto it = std::find_if(this->_groups.begin(), this->_groups.end(), [&quality](const std::shared_ptr<WebXClientGroup> & group) {
            return group->getQuality() == quality;
        });

        return (it != this->_groups.end()) ? *it : nullptr;
    }

    std::shared_ptr<WebXClientGroup> getOrCreateGroupByQuality(const WebXQuality & quality) {
        auto it = std::find_if(this->_groups.begin(), this->_groups.end(), [&quality](const std::shared_ptr<WebXClientGroup> & group) {
            return group->getQuality() == quality;
        });

        if (it == this->_groups.end()) {
            auto group = std::make_shared<WebXClientGroup>(this->_settings, quality);
            this->_groups.push_back(group);
            spdlog::debug("Created group with with quality index {:d}. Now have {:d} client groups", group->getQuality().index, this->_groups.size());
            return group;

        } else {
            return *it;
        }
    }

    std::shared_ptr<WebXClientGroup> getGroupWithClientId(uint32_t clientId) const {
        auto it = std::find_if(this->_groups.begin(), this->_groups.end(), [&clientId](const std::shared_ptr<WebXClientGroup> & group) {
            return group->getClientById(clientId) != nullptr;
        });

        return (it != this->_groups.end()) ? *it : nullptr;
    }

    void removeClientFromGroups(uint32_t clientId);
    void removeClientFromGroup(uint32_t clientId, const std::shared_ptr<WebXClientGroup> & group);


private:
    const WebXSettings & _settings;
    std::mt19937 _randomNumberGenerator;
    uint64_t _clientIndexMask;

    mutable std::recursive_mutex _mutex;

    std::vector<std::shared_ptr<WebXClient>> _clients;
    std::vector<std::shared_ptr<WebXClientGroup>> _groups;
};


#endif /* WEBX_CLIENT_REGISTRY_H */