#ifndef WEBX_INSTRUCTION_H
#define WEBX_INSTRUCTION_H

#include <string>
#include <nlohmann/json.hpp>

class WebXInstruction {
public:
    enum Type {
        Connect = 1,
        Image,
        Images,
        Window,
        Windows,
        Mouse
    };

    WebXInstruction() {}
    virtual ~WebXInstruction() {}

    Type type;
    std::string stringPayload;
    long numericPayload;
};

void to_json(nlohmann::json& j, const WebXInstruction & request);
void from_json(const nlohmann::json& j, WebXInstruction & request);


#endif /* WEBX_INSTRUCTION_H */