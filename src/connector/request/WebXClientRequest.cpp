#include "WebXClientRequest.h"


void to_json(nlohmann::json& j, const WebXClientRequest & request) {
    j = nlohmann::json{{"type", request.type}, {"stringPayload", request.stringPayload}, {"integerPayload", request.integerPayload}};
}

void from_json(const nlohmann::json& j, WebXClientRequest & request) {
    try {j.at("type").get_to(request.type); } catch (...) {}
    try {j.at("stringPayload").get_to(request.stringPayload); } catch (...) {}
    try {j.at("integerPayload").get_to(request.integerPayload); } catch (...) {}
}