#ifndef WEBX_CONNECTION_H
#define WEBX_CONNECTION_H

#include <memory>

class WebXMessage;

class WebXConnection {
public:
    WebXConnection() {}
    virtual ~WebXConnection() {}

    virtual void onMessage(std::shared_ptr<WebXMessage> message) = 0;
};


#endif /* WEBX_CONNECTION_H */