#ifndef WEBX_RESPONSE_H
#define WEBX_RESPONSE_H

#include <nlohmann/json.hpp>

class WebXResponse {
public:
    WebXResponse() {}
    virtual ~WebXResponse() {}

    virtual void toJson(nlohmann::json &) const = 0;
};


#endif /* WEBX_RESPONSE_H */