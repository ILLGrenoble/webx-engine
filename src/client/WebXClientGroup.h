#ifndef WEBX_CLIENT_GROUP_H
#define WEBX_CLIENT_GROUP_H

#include <vector>
#include <memory>
#include "WebXClient.h"
#include <utils/WebXQuality.h>

class WebXClientGroup {

public:
    WebXClientGroup(const WebXQuality & quality);
    virtual ~WebXClientGroup();

    const WebXQuality & getQuality() const {
        return this->_quality;
    }

    std::shared_ptr<WebXClient> getClientById(uint32_t id) const {
        auto it = std::find_if(this->_clients.begin(), this->_clients.end(), [&id](const std::shared_ptr<WebXClient> & client) {
            return client->getId() == id;
        });

        return (it != this->_clients.end()) ? *it : nullptr;
    }

    void addClient(const std::shared_ptr<WebXClient> & client) {
        if (this->getClientById(client->getId()) == nullptr) {
            this->_clients.push_back(client);
        }
    }

    void removeClient(uint32_t clientId) {
        this->_clients.erase(std::remove_if(this->_clients.begin(), this->_clients.end(), [&clientId](const std::shared_ptr<WebXClient> & client) { 
            return client->getId() == clientId;
        }), this->_clients.end());
    }

    bool isEmpty() const {
        return this->_clients.empty();
    }

private:
    const WebXQuality & _quality;

    std::vector<std::shared_ptr<WebXClient>> _clients;

};


#endif /* WEBX_CLIENT_GROUP_H */