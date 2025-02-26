#ifndef WEBX_SETTINGS_H
#define WEBX_SETTINGS_H

#include <cstdlib>
#include <string>
#include <spdlog/spdlog.h>


static int webx_settings_env_or_default(const std::string & envVarName, int defaultValue) {
    const char * envVar = std::getenv(envVarName.c_str());
    if (envVar) {
        int value = atoi(envVar);
        if (value > 0) {
            return value;

        } else {
            spdlog::warn("Failed to convert env var to int for {}: {}", envVarName, envVar);
        }
    }
    return defaultValue;
}

static bool webx_settings_env_or_default(const std::string & envVarName, bool defaultValue) {
    const char * envVar = std::getenv(envVarName.c_str());
    if (envVar) {
        return std::string(envVar) == "true";
    }
    return defaultValue;
}

static std::string webx_settings_env_or_default(const std::string & envVarName, const char * defaultValue) {
    const char * envVar = std::getenv(envVarName.c_str());
    if (envVar) {
        std::string value = std::string(envVar);
        return value;
    }
    return defaultValue;
}


class WebXQualitySettings {
public:
    enum CoverageQualityFunc {
        Disabled = 0,
        Linear,
        Quadratic,
    };
public:
    WebXQualitySettings() : 
        increaseQualityOnMouseOver(webx_settings_env_or_default("WEBX_ENGINE_INCREASED_QUALITY_ON_MOUSE_OVER", true)),
        coverageQualityFunc(convertCoverageQualityFuncString(webx_settings_env_or_default("WEBX_ENGINE_COVERAGE_QUALITY_FUNC", "quadratic"))),
        limitQualityByDataRate(webx_settings_env_or_default("WEBX_ENGINE_LIMIT_QUALITY_BY_DATA_RATE", true)) {}

    bool increaseQualityOnMouseOver;
    CoverageQualityFunc coverageQualityFunc;
    bool limitQualityByDataRate;

private:
    CoverageQualityFunc convertCoverageQualityFuncString(const std::string & coverageQualityFuncString) {
        if (coverageQualityFuncString == "linear") {
            return Linear;
        } else if (coverageQualityFuncString == "quadratic") {
            return Quadratic;
        }
        return Disabled;
    }

};

class WebXTransportSettings {
public:
    WebXTransportSettings() :
        connectorPort(webx_settings_env_or_default("WEBX_ENGINE_CONNECTOR_PORT", 5555)),
        collectorPort(webx_settings_env_or_default("WEBX_ENGINE_COLLECTOR_PORT", 5556)),
        publisherPort(webx_settings_env_or_default("WEBX_ENGINE_PUBLISHER_PORT", 5557)),
        ipcMessageProxyPath(webx_settings_env_or_default("WEBX_ENGINE_IPC_MESSAGE_PROXY_PATH", "/tmp/webx-router-message-proxy.ipc")),
        ipcInstructionProxyPath(webx_settings_env_or_default("WEBX_ENGINE_IPC_INSTRUCTION_PROXY_PATH", "/tmp/webx-router-instruction-proxy.ipc")),
        ipcSessionConnectorPath(webx_settings_env_or_default("WEBX_ENGINE_IPC_SESSION_CONNECTOR_PATH", "/tmp/webx-engine-session-connector.ipc")),
        inprocEventBusAddress(webx_settings_env_or_default("WEBX_ENGINE_INPROC_EVENT_BUS_ADDRESS", "inproc://webx-engine/event-bus")),
        sessionId(convertSessionIdStringToBytes(webx_settings_env_or_default("WEBX_ENGINE_SESSION_ID", "00000000000000000000000000000000"))),
        sessionIdString(webx_settings_env_or_default("WEBX_ENGINE_SESSION_ID", "00000000000000000000000000000000")) {
    }

    std::array<unsigned char, 16> convertSessionIdStringToBytes(const std::string & sessionIdString) {
        if (sessionIdString.length() == 32) {
            std::array<unsigned char, 16> sessionId;
            for (unsigned int stringPos = 0, i = 0; stringPos < sessionIdString.length(); stringPos += 2, i++) {
                std::string byteString = sessionIdString.substr(stringPos, 2);
                char byte = (char) strtol(byteString.c_str(), NULL, 16);
                sessionId[i] = byte;
            }
            return sessionId;

        } else {
            spdlog::error("Session Id is not a valid UUID: {}", sessionIdString);
            exit(1);
        }
    }

    const int connectorPort;
    const int collectorPort;
    const int publisherPort;

    const std::string ipcMessageProxyPath;
    const std::string ipcInstructionProxyPath;
    const std::string ipcSessionConnectorPath;
    
    const std::string inprocEventBusAddress;

    const std::array<unsigned char, 16> sessionId;
    const std::string sessionIdString;
};

class WebXSettings {
public:
    WebXSettings() :
        logging(webx_settings_env_or_default("WEBX_ENGINE_LOG", "info")) {
    }

    const std::string logging;
    const WebXTransportSettings transport;
    const WebXQualitySettings quality;

};

#endif /* WEBX_SETTINGS_H */
