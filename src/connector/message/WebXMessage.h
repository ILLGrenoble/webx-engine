#ifndef WEBX_MESSAGE_H
#define WEBX_MESSAGE_H

#include <nlohmann/json.hpp>

class WebXMessage {
public:
    WebXMessage() {}
    virtual ~WebXMessage() {}

    virtual void toJson(nlohmann::json & j) const = 0;
};


#endif /* WEBX_MESSAGE_H */