#ifndef WEBX_CONNECTION_MESSAGE_H
#define WEBX_CONNECTION_MESSAGE_H

#include "WebXMessage.h"
#include <utils/WebXSize.h>

class WebXConnectionMessage : public WebXMessage {
public:
    WebXConnectionMessage(int publisherPort, int collectorPort, WebXSize screenSize) :
        _publisherPort(publisherPort),
        _collectorPort(collectorPort),
        _screenSize(screenSize) {}
    
    virtual ~WebXConnectionMessage() {}

    virtual void toJson(nlohmann::json & j) const {
        j = nlohmann::json{
            {"type", "connection"},
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


#endif /* WEBX_CONNECTION_MESSAGE_H */