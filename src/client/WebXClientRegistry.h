#ifndef WEBX_CLIENT_REGISTRY_H
#define WEBX_CLIENT_REGISTRY_H

#include <vector>
#include <memory>
#include <random>

#include "WebXClient.h"
#include "WebXClientGroup.h"
#include <utils/WebXResult.h>
#include <utils/WebXQuality.h>

class WebXClientRegistry {

public:
    WebXClientRegistry();
    virtual ~WebXClientRegistry();

    const WebXResult<std::pair<uint32_t, uint64_t>> addClient();
    const WebXResult<void> removeClient(uint32_t clientId);

private:
    std::shared_ptr<WebXClient> getClientById(uint32_t id) {
        auto it = std::find_if(this->_clients.begin(), this->_clients.end(), [&id](const std::shared_ptr<WebXClient> & client) {
            return client->getId() == id;
        });

        return (it != this->_clients.end()) ? *it : nullptr;
    }

    std::shared_ptr<WebXClientGroup> getGroupByQuality(const WebXQuality & quality) {
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
            auto group = std::make_shared<WebXClientGroup>(quality);
            this->_groups.push_back(group);
            return group;

        } else {
            return *it;
        }
    }

    std::shared_ptr<WebXClientGroup> getGroupWithClientId(uint32_t clientId) {
        auto it = std::find_if(this->_groups.begin(), this->_groups.end(), [&clientId](const std::shared_ptr<WebXClientGroup> & group) {
            return group->getClientById(clientId) != nullptr;
        });

        return (it != this->_groups.end()) ? *it : nullptr;
    }

    void removeClientFromGroups(uint32_t clientId);


private:
    std::mt19937 _randomNumberGenerator;
    uint64_t _clientIndexMask;

    std::mutex _mutex;

    std::vector<std::shared_ptr<WebXClient>> _clients;
    std::vector<std::shared_ptr<WebXClientGroup>> _groups;
};


#endif /* WEBX_CLIENT_REGISTRY_H */