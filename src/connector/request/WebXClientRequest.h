#ifndef WEBX_CLIENT_REQUEST_H
#define WEBX_CLIENT_REQUEST_H

#include <string>
#include <nlohmann/json.hpp>

struct WebXClientRequest {
    enum Type {
        Connect = 1,
        Image,
        Images,
        Window,
        Windows,
        Mouse
    };

    Type type;
    std::string stringPayload;
    long numericPayload;
};

void to_json(nlohmann::json& j, const WebXClientRequest & request);

void from_json(const nlohmann::json& j, WebXClientRequest & request);

#endif /* WEBX_CLIENT_REQUEST_H */