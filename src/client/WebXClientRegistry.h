#ifndef WEBX_CLIENT_REGISTRY_H
#define WEBX_CLIENT_REGISTRY_H

#include <vector>
#include <memory>

#include <utils/WebXResult.h>
class WebXClientGroup;

class WebXClientRegistry {

public:
    WebXClientRegistry();
    virtual ~WebXClientRegistry();

    const WebXResult<std::pair<uint32_t, uint64_t>> addClient();

    void removeClient(uint32_t clientId);

private:
    std::vector<std::shared_ptr<WebXClientGroup>> _client;
    std::vector<std::shared_ptr<WebXClientGroup>> _groups;
    uint64_t _clientIndexMask;

};


#endif /* WEBX_CLIENT_REGISTRY_H */