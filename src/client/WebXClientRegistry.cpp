#include "WebXClientRegistry.h"
#include "WebXClient.h"
#include <spdlog/spdlog.h>

WebXClientRegistry::WebXClientRegistry(const WebXSettings & settings) :
    _settings(settings),
    _randomNumberGenerator(std::random_device{}()),
    _clientIndexMask(0) {

}

WebXClientRegistry::~WebXClientRegistry() {

}

const WebXResult<std::pair<uint32_t, uint64_t>> WebXClientRegistry::addClient() {
    const std::lock_guard<std::recursive_mutex> lock(this->_mutex);

    // Check we have available indices
    if (~this->_clientIndexMask == 0) {
        return WebXResult<std::pair<uint32_t, uint64_t>>::Err("no client indices available");
    }

    // Get next available index
    uint64_t clientIndex = 1;
    while ((clientIndex & this->_clientIndexMask) != 0) {
        clientIndex <<= 1;
    }

    // Get unique Id
    uint32_t clientId;
    do {
        std::uniform_int_distribution<uint32_t> dist(0, UINT32_MAX);
        clientId = dist(this->_randomNumberGenerator);

    } while (this->getClientById(clientId) != nullptr);

    // Create client and add index to mask
    const std::shared_ptr<WebXClient> & client = std::make_shared<WebXClient>(clientId, clientIndex);
    this->_clients.push_back(client);
    this->_clientIndexMask |= clientIndex;

    // Add to default group (create group if needed)
    const WebXQuality & defaultQuality = WebXQuality::MaxQuality();
    const std::shared_ptr<WebXClientGroup> & group = this->getOrCreateGroupByQuality(defaultQuality);
    group->addClient(client);

    spdlog::debug("Added client with Id {:08x} and index {:016x} and added to default group. Now have {:d} clients connected", clientId, clientIndex, this->_clients.size());

    // Return identifier clientid and index
    return WebXResult<std::pair<uint32_t, uint64_t>>::Ok(std::pair<uint32_t, uint64_t>(clientId, clientIndex));
}

const WebXResult<void> WebXClientRegistry::removeClient(uint32_t clientId) {
    const std::lock_guard<std::recursive_mutex> lock(this->_mutex);

    // Determine if client exists in vector and remove it
    const std::shared_ptr<WebXClient> & client = this->getClientById(clientId);
    if (client != nullptr) {
        auto it = std::find(this->_clients.begin(), this->_clients.end(), client);
        if (it != this->_clients.end()) {
            this->_clients.erase(it);
        }
        spdlog::debug("Removed client with Id {:08x} and index {:016x}. Now have {:d} clients connected", clientId, client->getIndex(), this->_clients.size());

        // Remove clientIndex bits from mask
        this->_clientIndexMask &= ~client->getIndex();

        this->removeClientFromGroups(clientId);

        return WebXResult<void>::Ok();

    } else {
        spdlog::debug("Cannot remove unknown client with Id {:08x}", clientId);

        // Ensure that client doesn't exist in group anyway
        this->removeClientFromGroups(clientId);

        return WebXResult<void>::Err("client id is unknown");
    }
}

void WebXClientRegistry::removeClientFromGroups(uint32_t clientId) {
    // Find associated group and remove client from it
    const std::shared_ptr<WebXClientGroup> & group = this->getGroupWithClientId(clientId);
    if (group != nullptr) {
        group->removeClient(clientId);

        // If group is empty then remove it
        if (group->hasClients()) {
            auto it = std::find(this->_groups.begin(), this->_groups.end(), group);
            if (it != this->_groups.end()) {
                this->_groups.erase(it);
            }

            spdlog::debug("Removed empty group with with quality index {:d}. Now have {:d} client groups", group->getQuality().index, this->_groups.size());
        }
    }
}
