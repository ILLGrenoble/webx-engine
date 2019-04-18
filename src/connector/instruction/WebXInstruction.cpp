#include "WebXInstruction.h"

void to_json(nlohmann::json& j, const WebXInstruction & instruction) {
    j = nlohmann::json{{"type", instruction.type}, {"stringPayload", instruction.stringPayload}, {"numericPayload", instruction.numericPayload}};
}

void from_json(const nlohmann::json& j, WebXInstruction & instruction) {
    try {j.at("type").get_to(instruction.type); } catch (...) {}
    try {j.at("stringPayload").get_to(instruction.stringPayload); } catch (...) {}
    try {j.at("numericPayload").get_to(instruction.numericPayload); } catch (...) {}
}