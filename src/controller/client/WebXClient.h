#ifndef WEBX_CLIENT_H
#define WEBX_CLIENT_H

#include <memory>
#include <chrono>
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

public:
    WebXClient(uint32_t id, uint64_t index) :
        _id(id),
        _index(index),
        _pingStatus(PingStatus::WaitingToPing),
        _pingSentTime(std::chrono::high_resolution_clock::now()),
        _pongReceivedTime(std::chrono::high_resolution_clock::now()),
        _bitrateRatio(WebXOptional<float>::Empty()),
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

    WebXOptional<float> calculateAverageBitrateMbps() {
        return this->_bitrateCalculator.calculateAverageBitrateMbps();
    }

    float getAverageRTTLatencyMs() const {
        return this->_bitrateCalculator.getAverageRTTLatencyMs();
    }

    void resetBitrateData() {
        this->_bitrateCalculator.resetBitrateData();
        this->_bitrateRatio = WebXOptional<float>::Empty();
    }

    const WebXOptional<float> & getBitrateRatio() const {
        return this->_bitrateRatio;
    }

    void performQualityVerification(const WebXOptional<float> & averageImageMbps) {
        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float, std::milli> durationMs = now - this->_lastQualityVerificationTime;
    
        if (averageImageMbps.hasValue() && durationMs.count() >= QUALITY_VERIFICATION_PERIOD_MS) {
    
            WebXOptional<float> clientDataRate = this->calculateAverageBitrateMbps();
    
            if (clientDataRate.hasValue()) {
                spdlog::debug("Client {:08x}: Sending data at {:.2f} Mbps to client that receives at {:.2f} Mbps with {:.0f} ms latency", this->_id, averageImageMbps.value(), clientDataRate.value(), this->getAverageRTTLatencyMs());
                this->_bitrateRatio = WebXOptional<float>::Value(averageImageMbps.value() / clientDataRate.value());
            
            } else {
                this->_bitrateRatio = WebXOptional<float>::Empty();
            }
    
            this->_lastQualityVerificationTime = now;
        
        } else {
            this->_bitrateRatio = WebXOptional<float>::Empty();
        }
    }    
private:
    const static int PING_WAIT_INTERVAL_MS = 2000;
    const static int PONG_RESPONSE_TIMEOUT_MS = 10000;
    const static int QUALITY_VERIFICATION_PERIOD_MS = 2000;

    uint32_t _id;
    uint64_t _index;

    PingStatus _pingStatus;
    std::chrono::high_resolution_clock::time_point _pingSentTime;
    std::chrono::high_resolution_clock::time_point _pongReceivedTime;

    WebXClientBitrateCalculator _bitrateCalculator;
    WebXOptional<float> _bitrateRatio;
    std::chrono::high_resolution_clock::time_point _lastQualityVerificationTime;
};


#endif /* WEBX_CLIENT_H */