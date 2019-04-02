#ifndef WEBX_MESSAGE_H
#define WEBX_MESSAGE_H

#include <nlohmann/json.hpp>

class WebXMessage {
public:
    WebXMessage() {}
    virtual ~WebXMessage() {}

    virtual const nlohmann::json & getJson() const = 0;
};


#endif /* WEBX_MESSAGE_H */