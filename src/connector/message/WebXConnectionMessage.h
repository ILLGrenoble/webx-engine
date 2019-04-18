#ifndef WEBX_CONNECTION_MESSAGE_H
#define WEBX_CONNECTION_MESSAGE_H

#include "WebXMessage.h"
#include <utils/WebXSize.h>
#include <string>

class WebXConnectionMessage : public WebXMessage {
public:
    WebXConnectionMessage(int publisherPort, int collectorPort, std::string serializer, WebXSize screenSize) :
        WebXMessage("connection"),
        publisherPort(publisherPort),
        collectorPort(collectorPort),
        serializer(serializer),
        screenSize(screenSize) {}
    
    virtual ~WebXConnectionMessage() {}

    int publisherPort;
    int collectorPort;
    std::string serializer;
    WebXSize screenSize;
};


#endif /* WEBX_CONNECTION_MESSAGE_H */