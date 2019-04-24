#ifndef WEBX_CONNECTION_MESSAGE_H
#define WEBX_CONNECTION_MESSAGE_H

#include "WebXMessage.h"
#include <utils/WebXSize.h>
#include <string>

class WebXConnectionMessage : public WebXMessage {
public:
    WebXConnectionMessage(int publisherPort, int collectorPort) :
        WebXMessage(Type::Connection),
        publisherPort(publisherPort),
        collectorPort(collectorPort) {}
    
    virtual ~WebXConnectionMessage() {}

    int publisherPort;
    int collectorPort;
};


#endif /* WEBX_CONNECTION_MESSAGE_H */