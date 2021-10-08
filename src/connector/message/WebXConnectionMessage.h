#ifndef WEBX_CONNECTION_MESSAGE_H
#define WEBX_CONNECTION_MESSAGE_H

#include "WebXMessage.h"
#include <utils/WebXSize.h>
#include <string>

class WebXConnectionMessage : public WebXMessage {
public:
    WebXConnectionMessage(int32_t publisherPort, int32_t collectorPort) :
        WebXMessage(Type::Connection),
        publisherPort(publisherPort),
        collectorPort(collectorPort) {}
    
    virtual ~WebXConnectionMessage() {}

    int32_t publisherPort;
    int32_t collectorPort;
};


#endif /* WEBX_CONNECTION_MESSAGE_H */