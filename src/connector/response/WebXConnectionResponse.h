#ifndef WEBX_CONNECTION_RESPONSE_H
#define WEBX_CONNECTION_RESPONSE_H

#include "WebXClientConnectorResponse.h"
#include <utils/WebXSize.h>

class WebXConnectionResponse : public WebXClientConnectorResponse {
public:
    WebXConnectionResponse(int publisherPort, int collectorPort, WebXSize screenSize) {
        this->_connectionDetails = nlohmann::json{
            {"publisherPort", publisherPort}, 
            {"collectorPort", collectorPort}, 
            {"screenSize", {
                {"width", screenSize.width},
                {"height", screenSize.height}
            }}
        };
    }
    virtual ~WebXConnectionResponse() {}

    virtual const nlohmann::json & getJson() const {
        return this->_connectionDetails;
    }

private:
    nlohmann::json _connectionDetails;
};


#endif /* WEBX_CONNECTION_RESPONSE_H */