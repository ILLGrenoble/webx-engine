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

class WebXWindowVisibility;

class WebXClientGroup {
public:
    WebXClientGroup(const WebXSettings & settings, const WebXQuality & quality);
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

            // Add client and update client index mask
            this->_clients.push_back(client);
            this->_clientIndexMask |= client->getIndex();
        }
    }

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

    bool hasClients() const {
        return this->_clients.empty();
    }

    void updateVisibleWindows(const std::vector<const WebXWindowVisibility *> & windowVisibilities);

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

    void handleWindowDamage(std::function<WebXResult<WebXWindowImageTransferData>(const std::unique_ptr<WebXClientWindow> & window, uint64_t clientIndexMask)> damageHandlerFunc);

private:
    const WebXSettings & _settings;
    const WebXQuality & _quality;
    uint64_t _clientIndexMask;

    std::vector<std::shared_ptr<WebXClient>> _clients;
    std::vector<std::unique_ptr<WebXClientWindow>> _windows;
};


#endif /* WEBX_CLIENT_GROUP_H */