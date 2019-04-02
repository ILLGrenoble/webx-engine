#ifndef WEBX_CLIENT_CONNECTOR_RESPONSE_H
#define WEBX_CLIENT_CONNECTOR_RESPONSE_H

#include <nlohmann/json.hpp>

class WebXClientConnectorResponse {
public:
    WebXClientConnectorResponse() {}
    virtual ~WebXClientConnectorResponse() {}

    virtual const nlohmann::json & getJson() const = 0;
};


#endif /* WEBX_CLIENT_CONNECTOR_RESPONSE_H */