#ifndef WEBX_CLIENT_CONNECTOR_RESPONSE_H
#define WEBX_CLIENT_CONNECTOR_RESPONSE_H

#include <string>
#include <nlohmann/json.hpp>

class WebXClientConnectorResponse {
public:
    WebXClientConnectorResponse() {}
    virtual ~WebXClientConnectorResponse() {}

    virtual void toJson(nlohmann::json& j) const = 0;
};


#endif /* WEBX_CLIENT_CONNECTOR_RESPONSE_H */