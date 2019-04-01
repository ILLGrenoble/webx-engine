#ifndef WEBX_CONNECTION_RESPONSE_H
#define WEBX_CONNECTION_RESPONSE_H

#include "WebXClientConnectorResponse.h"
#include <utils/WebXSize.h>

class WebXConnectionResponse : public WebXClientConnectorResponse {
public:
    WebXConnectionResponse(int publisherPort, int collectorPort, WebXSize screenSize) :
        _publisherPort(publisherPort),
        _collectorPort(collectorPort),
        _screenSize(screenSize) {}
    virtual ~WebXConnectionResponse() {}

    virtual void toJson(nlohmann::json& j) const {
        j = nlohmann::json{
            {"publisherPort", this->_publisherPort}, 
            {"collectorPort", this->_collectorPort}, 
            {"screenSize", {
                {"width", this->_screenSize.width},
                {"height", this->_screenSize.height}
            }}
        };
    }

private:
    int _publisherPort;
    int _collectorPort;
    WebXSize _screenSize;
};


#endif /* WEBX_CONNECTION_RESPONSE_H */