#ifndef WEBX_CONNECTION_MESSAGE_H
#define WEBX_CONNECTION_MESSAGE_H

#include "WebXMessage.h"
#include <utils/WebXSize.h>

class WebXConnectionMessage : public WebXMessage {
public:
    WebXConnectionMessage(int publisherPort, int collectorPort, WebXSize screenSize) :
        WebXMessage("connection"),
        publisherPort(publisherPort),
        collectorPort(collectorPort),
        screenSize(screenSize) {}
    
    virtual ~WebXConnectionMessage() {}

    int publisherPort;
    int collectorPort;
    WebXSize screenSize;
};


#endif /* WEBX_CONNECTION_MESSAGE_H */