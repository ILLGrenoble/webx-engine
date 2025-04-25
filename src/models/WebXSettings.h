#ifndef WEBX_SETTINGS_H
#define WEBX_SETTINGS_H

#include <cstdlib>
#include <string>
#include <spdlog/spdlog.h>
#include <models/WebXQuality.h>

/* 
 * Utility function to get an integer value from an environment variable 
 * or use a default value if the variable is not set or invalid.
 */
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

/* 
 * Utility function to get a boolean value from an environment variable 
 * or use a default value if the variable is not set.
 */
static bool webx_settings_env_or_default(const std::string & envVarName, bool defaultValue) {
    const char * envVar = std::getenv(envVarName.c_str());
    if (envVar) {
        return std::string(envVar) == "true";
    }
    return defaultValue;
}

/* 
 * Utility function to get a string value from an environment variable 
 * or use a default value if the variable is not set.
 */
static std::string webx_settings_env_or_default(const std::string & envVarName, const char * defaultValue) {
    const char * envVar = std::getenv(envVarName.c_str());
    if (envVar) {
        std::string value = std::string(envVar);
        return value;
    }
    return defaultValue;
}

/* 
 * Class to manage quality-related settings for WebX.
 * Includes options for increasing quality on mouse over, 
 * selecting a coverage quality function, and limiting quality by data rate.
 */
class WebXQualitySettings {
public:
    /* 
     * Enum to define different coverage quality functions.
     */
    enum CoverageQualityFunc {
        Disabled = 0,  /* Quality function is disabled */
        Linear,        /* Linear quality function */
        Quadratic,     /* Quadratic quality function */
    };

public:
    /* 
     * Constructor initializes settings from environment variables or defaults.
     */
    WebXQualitySettings() : 
        increaseQualityOnMouseOver(webx_settings_env_or_default("WEBX_ENGINE_INCREASE_QUALITY_ON_MOUSE_OVER", true)),
        coverageQualityFunc(convertCoverageQualityFuncString(webx_settings_env_or_default("WEBX_ENGINE_COVERAGE_QUALITY_FUNC", "quadratic"))),
        limitQualityByDataRate(webx_settings_env_or_default("WEBX_ENGINE_LIMIT_QUALITY_BY_DATA_RATE", true)),
        runtimeMaxQualityIndex(webx_settings_env_or_default("WEBX_ENGINE_RUNTIME_MAX_QUALITY_INDEX", 12)) {
            WebXQuality::SetRuntimeMaxQualityIndex(runtimeMaxQualityIndex);
        }

    const bool increaseQualityOnMouseOver;
    const CoverageQualityFunc coverageQualityFunc;
    const bool limitQualityByDataRate;
    const int runtimeMaxQualityIndex;

private:
    /* 
     * Helper function to convert a string to a CoverageQualityFunc enum.
     */
    CoverageQualityFunc convertCoverageQualityFuncString(const std::string & coverageQualityFuncString) {
        if (coverageQualityFuncString == "linear") {
            return Linear;
        } else if (coverageQualityFuncString == "quadratic") {
            return Quadratic;
        }
        return Disabled;
    }

};

/* 
 * Class to manage transport-related settings for WebX.
 * Includes configuration for ports, IPC paths, and session identifiers.
 */
class WebXTransportSettings {
public:
    /* 
     * Constructor initializes settings from environment variables or defaults.
     */
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

    /* 
     * Helper function to convert a session ID string to a byte array.
     * Exits the program if the session ID is invalid.
     */
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

/**
 * Class to manage controller-related settings for WebX.
 * Includes configuration for image checksum verification.
 */
class WebXControllerSettings {
public:
    /* 
     * Constructor initializes settings from environment variables or defaults.
     */
    WebXControllerSettings(bool defaultImageCheckumEnabled) : 
        imageChecksumEnabled(webx_settings_env_or_default("WEBX_ENGINE_IMAGE_CHECKSUM_ENABLED", defaultImageCheckumEnabled)),
        clientPingResponseTimeoutMs(webx_settings_env_or_default("WEBX_ENGINE_CLIENT_PING_RESPONSE_TIMEOUT_MS", 15000)) {}

    const bool imageChecksumEnabled;
    const int clientPingResponseTimeoutMs;
};

/**
 * Class to manage event-related settings for WebX.
 * Includes configuration for filtering damage events after configure notify.
 */
class WebXEventSettings {
public:
    /* 
     * Constructor initializes settings from environment variables or defaults.
     */
    WebXEventSettings() : 
        filterDamageAfterConfigureNotify(webx_settings_env_or_default("WEBX_ENGINE_EVENT_FILTER_DAMAGE_AFTER_CONFIGURE_NOTIFY", true)) {}

    const bool filterDamageAfterConfigureNotify;
};

/* 
 * Class to manage overall settings for WebX.
 * Includes logging configuration, transport settings, and quality settings.
 */
class WebXSettings {
public:
    /* 
     * Constructor initializes settings from environment variables or defaults.
     */
    WebXSettings() :
        logLevel(webx_settings_env_or_default("WEBX_ENGINE_LOG_LEVEL", "debug")),
        event(WebXEventSettings()),
        controller(WebXControllerSettings(!this->event.filterDamageAfterConfigureNotify))  // default checksum enabled: true if filtering not enabled, false if filtering enabled
        { }

    const std::string logLevel;
    const WebXEventSettings event;
    const WebXControllerSettings controller;
    const WebXTransportSettings transport;
    const WebXQualitySettings quality;

};

#endif /* WEBX_SETTINGS_H */
