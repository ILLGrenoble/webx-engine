#ifndef WEBX_SETTINGS_H
#define WEBX_SETTINGS_H

#include <cstdlib>
#include <spdlog/spdlog.h>

class WebXSettings {
public:
    WebXSettings() :
        logging(envOrDefault("WEBX_ENGINE_LOG", "info")),
        connectorPort(envOrDefault("WEBX_ENGINE_CONNECTOR_PORT", 5555)),
        collectorPort(envOrDefault("WEBX_ENGINE_COLLECTOR_PORT", 5556)),
        publisherPort(envOrDefault("WEBX_ENGINE_PUBLISHER_PORT", 5557)),
        ipcMessageProxyPath(envOrDefault("WEBX_ENGINE_IPC_MESSAGE_PROXY_PATH", "/tmp/webx-router-message-proxy.ipc")),
        ipcInstructionProxyPath(envOrDefault("WEBX_ENGINE_IPC_INSTRUCTION_PROXY_PATH", "/tmp/webx-router-instruction-proxy.ipc")),
        ipcSessionConnectorPath(envOrDefault("WEBX_ENGINE_IPC_SESSION_CONNECTOR_PATH", "/tmp/webx-engine-session-connector.ipc")),
        inprocEventBusAddress(envOrDefault("WEBX_ENGINE_INPROC_EVENT_BUS_ADDRESS", "inproc://webx-engine/event-bus")),
        sessionId(envOrDefault("WEBX_ENGINE_SESSION_ID", ""))
    {}

    int envOrDefault(const std::string & envVarName, int defaultValue) {
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

    std::string envOrDefault(const std::string & envVarName, const std::string & defaultValue) {
        const char * envVar = std::getenv(envVarName.c_str());
        if (envVar) {
            std::string value = std::string(envVar);
            return value;
        }
        return defaultValue;
    }

    std::string logging;

    int connectorPort;
    int collectorPort;
    int publisherPort;

    std::string ipcMessageProxyPath;
    std::string ipcInstructionProxyPath;
    std::string ipcSessionConnectorPath;
    
    std::string inprocEventBusAddress;

    std::string sessionId;
};

#endif /* WEBX_SETTINGS_H */
