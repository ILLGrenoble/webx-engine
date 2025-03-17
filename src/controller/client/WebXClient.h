#ifndef WEBX_CLIENT_H
#define WEBX_CLIENT_H

#include <memory>
#include <chrono>
#include <vector>
#include <spdlog/spdlog.h>
#include "WebXClientBitrateCalculator.h"
#include <utils/WebXOptional.h>

class WebXClient {
public:
    enum PingStatus {
        WaitingToPing = 0,
        RequiresPing,
        WaitingForPong,
        PongTimeout,
    };

    class WebXClientBitrateMeans {
        public:
            WebXClientBitrateMeans() {}
            WebXClientBitrateMeans(float meanImageMbps, float meanBitrateMbps, float meanRTTLatencyMs, float meanBitrateRatio) :
                meanImageMbps(meanImageMbps),
                meanBitrateMbps(meanBitrateMbps),
                meanRTTLatencyMs(meanRTTLatencyMs),
                meanBitrateRatio(meanBitrateRatio) {}
            virtual ~WebXClientBitrateMeans() {}
    
            float meanImageMbps;
            float meanBitrateMbps;
            float meanRTTLatencyMs;
            float meanBitrateRatio;
        };

public:
    WebXClient(uint32_t id, uint64_t index) :
        _id(id),
        _index(index),
        _pingStatus(PingStatus::WaitingToPing),
        _pingSentTime(std::chrono::high_resolution_clock::now()),
        _pongReceivedTime(std::chrono::high_resolution_clock::now()),
        _bitrateMeans(WebXOptional<WebXClientBitrateMeans>::Empty()),
        _lastQualityVerificationTime(std::chrono::high_resolution_clock::now()) {}
    virtual ~WebXClient() {}

    uint32_t getId() const {
        return this->_id;
    }

    uint64_t getIndex() const {
        return this->_index;
    }

    void updatePingStatus() {
        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        if (this->_pingStatus == WaitingToPing) {
            std::chrono::duration<double, std::milli> timeSincePong = now - this->_pongReceivedTime;
            this->_pingStatus = timeSincePong.count() > PING_WAIT_INTERVAL_MS ? RequiresPing : WaitingToPing;
        
        } else if (this->_pingStatus == WaitingForPong) {
            std::chrono::duration<double, std::milli> timeSincePing = now - this->_pingSentTime;
            this->_pingStatus = timeSincePing.count() > PONG_RESPONSE_TIMEOUT_MS ? PongTimeout : WaitingForPong;
        }
    }

    PingStatus getPingStatus() const {
        return this->_pingStatus;
    }

    void onPingSent() {
        this->_pingSentTime = std::chrono::high_resolution_clock::now();
        this->_pingStatus = WaitingForPong;
    }

    void onPongReceived(uint64_t sendTimestampMs, uint64_t recvTimestampMs) {
        this->_pongReceivedTime = std::chrono::high_resolution_clock::now();
        this->_pingStatus = WaitingToPing;

        this->_bitrateCalculator.updateLatency(sendTimestampMs, recvTimestampMs);
    }

    void onDataAckReceived(uint64_t sendTimestampMs, uint64_t recvTimestampMs, uint32_t dataLength) {
        this->_bitrateCalculator.updateBitrateData(sendTimestampMs, recvTimestampMs, dataLength);
    }

    void setCurrentGroupImageMbps(const WebXOptional<float> & currentGroupImageMbps) {
        this->_bitrateCalculator.setCurrentGroupImageMbps(currentGroupImageMbps);
    }

    void resetBitrateData(const WebXOptional<float> & currentGroupImageMbps) {
        this->_bitrateCalculator.resetBitrateData(currentGroupImageMbps);
        this->_bitrateMeans = WebXOptional<WebXClientBitrateMeans>::Empty();
    }

    const WebXOptional<WebXClientBitrateMeans> & getBitrateMeans() const {
        return this->_bitrateMeans;
    }

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
        
                this->_lastQualityVerificationTime = now;
            }

        } else {
            this->_bitrateMeans = WebXOptional<WebXClientBitrateMeans>::Empty();
        }
    }    
private:
    const static int PING_WAIT_INTERVAL_MS = 1000;
    const static int PONG_RESPONSE_TIMEOUT_MS = 10000;
    const static int QUALITY_VERIFICATION_PERIOD_MS = 10000;

    uint32_t _id;
    uint64_t _index;

    PingStatus _pingStatus;
    std::chrono::high_resolution_clock::time_point _pingSentTime;
    std::chrono::high_resolution_clock::time_point _pongReceivedTime;

    WebXClientBitrateCalculator _bitrateCalculator;
    WebXOptional<WebXClientBitrateMeans> _bitrateMeans;
    std::chrono::high_resolution_clock::time_point _lastQualityVerificationTime;
};


#endif /* WEBX_CLIENT_H */