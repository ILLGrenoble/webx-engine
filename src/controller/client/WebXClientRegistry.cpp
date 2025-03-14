#include "WebXClientRegistry.h"
#include "WebXClient.h"
#include <models/message/WebXMessage.h>
#include <models/message/WebXPingMessage.h>
#include <models/message/WebXDisconnectMessage.h>
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
        this->removeClientFromGroup(clientId, group);
    }
}

void WebXClientRegistry::removeClientFromGroup(uint32_t clientId, const std::shared_ptr<WebXClientGroup> & group) {
    group->removeClient(clientId);

    // If group is empty then remove it
    if (group->hasClients()) {
        auto it = std::find(this->_groups.begin(), this->_groups.end(), group);
        if (it != this->_groups.end()) {
            this->_groups.erase(it);
        }

        spdlog::trace("Removed empty group with with quality index {:d}. Now have {:d} client groups", group->getQuality().index, this->_groups.size());
    }
}

void WebXClientRegistry::setClientQuality(uint32_t clientId, const WebXQuality & quality) {
    const std::lock_guard<std::recursive_mutex> lock(this->_mutex);

    // Determine if client exists
    const std::shared_ptr<WebXClient> & client = this->getClientById(clientId);
    if (client != nullptr) {
        // Find associated group and check if quality is different
        const std::shared_ptr<WebXClientGroup> & oldGroup = this->getGroupWithClientId(clientId);
        if (oldGroup == nullptr) {
            // shouldn't be here: a client should always be in a group
            const std::shared_ptr<WebXClientGroup> & group = this->getOrCreateGroupByQuality(quality);
            group->addClient(client);
        
            spdlog::trace("Added client with Id {:08x} and index {:016x} to group with quality index {:d}", clientId, client->getIndex(), quality.index);

        } else if (oldGroup != nullptr && oldGroup->getQuality() != quality) {
            this->removeClientFromGroup(clientId, oldGroup);

            const std::shared_ptr<WebXClientGroup> & group = this->getOrCreateGroupByQuality(quality);
            group->addClient(client);
        
            spdlog::debug("Moved client with Id {:08x} and index {:016x} from group with quality {:d} to {:d}", clientId, client->getIndex(), oldGroup->getQuality().index, quality.index);
        }
    }
}

void WebXClientRegistry::handleClientPings(const std::function<void(std::shared_ptr<WebXMessage> clientMessage)> clientMessageHandler) {
    const std::lock_guard<std::recursive_mutex> lock(this->_mutex);
    
    // Update client ping statuses
    for (auto & client : this->_clients) {
        client->updatePingStatus();
    }

    // Remove clients that have pong timeout
    std::vector<std::shared_ptr<WebXClient>> clientsWithTimeout;
    std::copy_if(this->_clients.begin(), this->_clients.end(), std::back_inserter(clientsWithTimeout), [](const std::shared_ptr<WebXClient> client) {
        return client->getPingStatus() == WebXClient::PongTimeout;
    });

    for (const std::shared_ptr<WebXClient> & client : clientsWithTimeout) {
        spdlog::debug("Ping Timeout for client with Id {:08x} and index {:016x}", client->getId(), client->getIndex());

        spdlog::trace("Sending Disconnect to client with Id {:08x} and index {:016x}", client->getId(), client->getIndex());
        auto message = std::make_shared<WebXDisconnectMessage>(client->getIndex());
        clientMessageHandler(message);

        this->removeClient(client->getId());
    }

    // Send pings to those that need
    for (auto & client : this->_clients) {
        if (client->getPingStatus() == WebXClient::RequiresPing) {
            spdlog::trace("Sending Ping to client with Id {:08x} and index {:016x}", client->getId(), client->getIndex());
            auto message = std::make_shared<WebXPingMessage>(client->getIndex());
            clientMessageHandler(message);
            client->onPingSent();
        }
    }
}

void WebXClientRegistry::performQualityVerification() {
    const std::lock_guard<std::recursive_mutex> lock(this->_mutex);
    for (auto & group : this->_groups) {
        group->performQualityVerification();
    }

    for (auto & client : this->_clients) {
        const std::shared_ptr<WebXClientGroup> & clientGroup = this->getGroupWithClientId(client->getId());
        const WebXQuality & quality = clientGroup->getQuality();
        const WebXOptional<WebXClient::WebXClientBitrateMeans> & bitrateMeans = client->getBitrateMeans();

        if (bitrateMeans.hasValue()) {
            float meanBitrateRatio = bitrateMeans.value().meanBitrateRatio;
            float meanBitrateMbps = bitrateMeans.value().meanBitrateMbps;
            float meanImageMbps = bitrateMeans.value().meanImageMbps;
            float meanRTTLatencyMs = bitrateMeans.value().meanRTTLatencyMs;
            
            int suggestedQualityDelta = 0;
            // if (meanBitrateRatio >= 1.0) {
            //     suggestedQualityDelta = -3;
            // } else if (meanBitrateRatio >= 0.9) {
            //     suggestedQualityDelta = -2;
            // } else 
            if (meanBitrateRatio >= 0.8) {
                suggestedQualityDelta = -1;

            // } else if (meanBitrateRatio < 0.1) {
            //     suggestedQualityDelta = +3;
            // } else if (meanBitrateRatio < 0.3) {
            //     suggestedQualityDelta = +2;
            } else if (meanBitrateRatio < 0.2) {
                suggestedQualityDelta = +1;
            }

            int suggestedQualityIndex = quality.index + suggestedQualityDelta;
            suggestedQualityIndex = suggestedQualityIndex < 1 ? 1 : suggestedQualityIndex > WebXQuality::MAX_QUALITY_INDEX ? WebXQuality::MAX_QUALITY_INDEX : suggestedQualityIndex;

            const WebXQuality & newQuality = WebXQuality::QualityForIndex(suggestedQualityIndex);
            if (newQuality != quality) {
                spdlog::info("Client {:08x}: {:s} quality to {:d} as bitrate ratio is {:s} (bitrate ratio = {:.2f}, image Mbps = {:.2f}, client bandwidth = {:.2f}, client RTT Latency = {:.0f})", client->getId(), suggestedQualityDelta < 0 ? "Reducing" : "Increasing", newQuality.index, suggestedQualityDelta < 0 ? "too high" : "low", meanBitrateRatio, meanImageMbps, meanBitrateMbps, meanRTTLatencyMs);
                this->setClientQuality(client->getId(), newQuality);
            }
        }

    }
}

