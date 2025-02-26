#include "WebXClientRegistry.h"
#include "WebXClient.h"
#include <spdlog/spdlog.h>

WebXClientRegistry::WebXClientRegistry() :
    _clientIndexMask(0) {

}

WebXClientRegistry::~WebXClientRegistry() {

}

const WebXResult<std::pair<uint32_t, uint64_t>> WebXClientRegistry::addClient() {
    // Check we have available indices
    if (~this->_clientIndexMask == 0) {
        return WebXResult<std::pair<uint32_t, uint64_t>>::Err("Tis naff");
    }

    // Get next available index

    // Get unique Id

    // Create client

    // Add to default group (create group if needed)

    // Return identifier (clientid:index)
    uint32_t clientId = 0x12345678;
    uint64_t clientIndex = 0x1000000000000000;
    spdlog::debug("Created client with id {} and index {}", clientId, clientIndex);

    return WebXResult<std::pair<uint32_t, uint64_t>>::Ok(std::pair<uint32_t, uint64_t>(clientId, clientIndex));

}

void WebXClientRegistry::removeClient(uint32_t clientId) {

}