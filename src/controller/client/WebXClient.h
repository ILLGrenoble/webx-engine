#ifndef WEBX_CLIENT_H
#define WEBX_CLIENT_H

#include <memory>
#include <chrono>
#include <vector>
#include <spdlog/spdlog.h>
#include "WebXClientBitrateCalculator.h"
#include <models/WebXQuality.h>
#include <utils/WebXOptional.h>

/**
 * @class WebXClient
 * @brief Represents a client and manages its quality, bitrate, and latency.
 * 
 * This class encapsulates the properties and behaviors of a client, including
 * quality verification, ping/pong handling, and bitrate calculations.
 */
class WebXClient {
public:
    enum PingStatus {
        WaitingToPing = 0,
        RequiresPing,
        WaitingForPong,
        PongTimeout,
    };

    /**
     * @class WebXClientBitrateMeans
     * @brief Represents the calculated means for bitrate-related metrics.
     * 
     * This class encapsulates the average metrics for image Mbps, bitrate Mbps,
     * round-trip time (RTT) latency in milliseconds, and bitrate ratio. These
     * metrics are used to evaluate and manage the client's network performance.
     */
    class WebXClientBitrateMeans {
    public:
        /**
         * @brief Default constructor for WebXClientBitrateMeans.
         * 
         * Initializes an empty instance of the class with default values.
         */
        WebXClientBitrateMeans() {}

        /**
         * @brief Parameterized constructor for WebXClientBitrateMeans.
         * 
         * Initializes the instance with the provided metrics.
         * 
         * @param meanImageMbps The mean image Mbps value.
         * @param meanBitrateMbps The mean bitrate Mbps value.
         * @param meanRTTLatencyMs The mean round-trip time latency in milliseconds.
         * @param meanBitrateRatio The mean bitrate ratio.
         */
        WebXClientBitrateMeans(float meanImageMbps, float meanBitrateMbps, float meanRTTLatencyMs, float meanBitrateRatio) :
            meanImageMbps(meanImageMbps),
            meanBitrateMbps(meanBitrateMbps),
            meanRTTLatencyMs(meanRTTLatencyMs),
            meanBitrateRatio(meanBitrateRatio) {}

        /**
         * @brief Destructor for WebXClientBitrateMeans.
         * 
         * Cleans up resources used by the instance.
         */
        virtual ~WebXClientBitrateMeans() {}

        float meanImageMbps;
        float meanBitrateMbps;
        float meanRTTLatencyMs;
        float meanBitrateRatio;
    };

public:
    /**
     * @brief Constructor for initializing a client with an ID, index, and maximum quality.
     * 
     * This constructor initializes the client with the provided unique identifier, index mask,
     * and maximum quality level. It also sets the initial ping status, timestamps, and bitrate means.
     * 
     * @param id The unique identifier of the client.
     * @param index The index mask associated with the client.
     * @param maxQuality The maximum quality level allowed for the client.
     * @param pingResponseTimeoutMs The timeout in milliseconds for receiving a ping response from a client
     */
    WebXClient(uint32_t id, uint64_t index, const WebXQuality & maxQuality, const int pingResponseTimeoutMs) :
        _id(id),
        _index(index),
        _maxQuality(maxQuality),
        _pingResponseTimeoutMs(pingResponseTimeoutMs),
        _pingStatus(PingStatus::WaitingToPing),
        _pingSentTime(std::chrono::high_resolution_clock::now()),
        _pongReceivedTime(std::chrono::high_resolution_clock::now()),
        _bitrateMeans(WebXOptional<WebXClientBitrateMeans>::Empty()),
        _lastQualityVerificationTime(std::chrono::high_resolution_clock::now()) {}
    
    /**
     * @brief Destructor for cleaning up resources.
     * 
     * This destructor ensures that any resources used by the client are properly released.
     */
    virtual ~WebXClient() {}

    /**
     * @brief Gets the unique identifier of the client.
     * 
     * This method returns the unique ID assigned to the client.
     * 
     * @return The client ID.
     */
    uint32_t getId() const {
        return this->_id;
    }

    /**
     * @brief Gets the index mask associated with the client.
     * 
     * This method returns the index mask that is used to identify messages for the client.
     * 
     * @return The client index mask.
     */
    uint64_t getIndex() const {
        return this->_index;
    }

    /**
     * @brief Gets the maximum quality level allowed for the client.
     * 
     * This method returns the maximum quality level that the client can handle.
     * 
     * @return A reference to the maximum quality object.
     */
    const WebXQuality & getMaxQuality() const {
        return this->_maxQuality;
    }

    /**
     * @brief Sets the maximum quality level for the client.
     * 
     * This method updates the maximum quality level that the client can handle.
     * 
     * @param quality The new maximum quality level.
     */
    void setMaxQuality(const WebXQuality & quality) {
        this->_maxQuality = quality;
    }

    /**
     * @brief Updates the ping status based on the current status and the elapsed time since the last ping or pong.
     * 
     * This method checks the elapsed time since the last ping or pong and updates the ping status accordingly.
     * If the client is waiting to ping, it checks if the interval has exceeded the wait time.
     * If the client is waiting for a pong, it checks if the response timeout has been exceeded.
     */
    void updatePingStatus() {
        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        if (this->_pingStatus == WaitingToPing) {
            std::chrono::duration<double, std::milli> timeSincePong = now - this->_pongReceivedTime;
            this->_pingStatus = timeSincePong.count() > PING_WAIT_INTERVAL_MS ? RequiresPing : WaitingToPing;
        
        } else if (this->_pingStatus == WaitingForPong) {
            std::chrono::duration<double, std::milli> timeSincePing = now - this->_pingSentTime;
            this->_pingStatus = timeSincePing.count() > this->_pingResponseTimeoutMs ? PongTimeout : WaitingForPong;
        }
    }

    /**
     * @brief Gets the current ping status of the client.
     * @return The current ping status.
     */
    PingStatus getPingStatus() const {
        return this->_pingStatus;
    }

    /**
     * @brief Marks the client as having sent a ping and updates the timestamp.
     */
    void onPingSent() {
        this->_pingSentTime = std::chrono::high_resolution_clock::now();
        this->_pingStatus = WaitingForPong;
    }

    /**
     * @brief Handles the reception of a pong message and updates latency metrics.
     * @param sendTimestampMs The timestamp when the ping was sent (in milliseconds).
     * @param recvTimestampMs The timestamp when the pong was received (in milliseconds).
     */
    void onPongReceived(uint64_t sendTimestampMs, uint64_t recvTimestampMs) {
        this->_pongReceivedTime = std::chrono::high_resolution_clock::now();
        this->_pingStatus = WaitingToPing;

        this->_bitrateCalculator.updateLatency(sendTimestampMs, recvTimestampMs);
    }

    /**
     * @brief Updates bitrate data upon receiving an acknowledgment for sent data.
     * @param sendTimestampMs The timestamp when the data was sent (in milliseconds).
     * @param recvTimestampMs The timestamp when the acknowledgment was received (in milliseconds).
     * @param dataLength The length of the data sent (in bytes).
     */
    void onDataAckReceived(uint64_t sendTimestampMs, uint64_t recvTimestampMs, uint32_t dataLength) {
        this->_bitrateCalculator.updateBitrateData(sendTimestampMs, recvTimestampMs, dataLength);
    }

    /**
     * @brief Sets the current group image Mbps for bitrate calculations.
     * @param currentGroupImageMbps The current group image Mbps value.
     */
    void setCurrentGroupImageMbps(const WebXOptional<float> & currentGroupImageMbps) {
        this->_bitrateCalculator.setCurrentGroupImageMbps(currentGroupImageMbps);
    }

    /**
     * @brief Resets bitrate data and clears the calculated means.
     * @param currentGroupImageMbps The current group image Mbps value.
     */
    void resetBitrateData(const WebXOptional<float> & currentGroupImageMbps) {
        this->_bitrateCalculator.resetBitrateData(currentGroupImageMbps);
        this->_bitrateMeans = WebXOptional<WebXClientBitrateMeans>::Empty();
    }

    /**
     * @brief Gets the calculated bitrate means for the client.
     * @return The optional bitrate means.
     */
    const WebXOptional<WebXClientBitrateMeans> & getBitrateMeans() const {
        return this->_bitrateMeans;
    }

    /**
     * @brief Performs quality verification based on average image Mbps and updates bitrate means.
     * @param averageImageMbps The average image Mbps value.
     */
    void performQualityVerification(const WebXOptional<float> & averageImageMbps) {
        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float, std::milli> durationMs = now - this->_lastQualityVerificationTime;
    
        if (durationMs.count() >= QUALITY_VERIFICATION_PERIOD_MS) {
    
            this->_bitrateCalculator.calculateAverageBitrateData();
            const WebXOptional<float> & meanBitrateMbps = this->_bitrateCalculator.getMeanBitrateMbps();
            const WebXOptional<float> & meanBitrateRatio = this->_bitrateCalculator.getMeanBitrateRatio();
            const WebXOptional<float> & meanRTTLatencyMs = this->_bitrateCalculator.getMeanRTTLatencyMs();

            if (averageImageMbps.hasValue() && meanBitrateMbps.hasValue() && meanBitrateRatio.hasValue() && meanRTTLatencyMs.hasValue()) {
                this->_bitrateMeans = WebXOptional<WebXClientBitrateMeans>::Value(WebXClientBitrateMeans(averageImageMbps.value(), meanBitrateMbps.value(), meanRTTLatencyMs.value(), meanBitrateRatio.value()));

                spdlog::trace("Client {:08x}: Sending data at {:.2f} Mbps to client with bandwidth {:.2f} Mbps and latency {:.0f} (bitrate ratio = {:.2f})", 
                    this->_id, averageImageMbps.value(), meanBitrateMbps.value(), meanRTTLatencyMs.value(), meanBitrateRatio.value());
        
            }
            this->_lastQualityVerificationTime = now;

        } else {
            this->_bitrateMeans = WebXOptional<WebXClientBitrateMeans>::Empty();
        }
    }    
private:
    const static int PING_WAIT_INTERVAL_MS = 1000;
    const static int QUALITY_VERIFICATION_PERIOD_MS = 10000;

    const uint32_t _id;
    const uint64_t _index;
    WebXQuality _maxQuality;
    const int _pingResponseTimeoutMs;

    PingStatus _pingStatus;
    std::chrono::high_resolution_clock::time_point _pingSentTime;
    std::chrono::high_resolution_clock::time_point _pongReceivedTime;

    WebXClientBitrateCalculator _bitrateCalculator;
    WebXOptional<WebXClientBitrateMeans> _bitrateMeans;
    std::chrono::high_resolution_clock::time_point _lastQualityVerificationTime;
};


#endif /* WEBX_CLIENT_H */