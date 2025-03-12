#ifndef WEBX_CLIENT_BITRATE_CALCULATOR_H
#define WEBX_CLIENT_BITRATE_CALCULATOR_H

#include <chrono>
#include <spdlog/spdlog.h>
#include <utils/WebXOptional.h>

class WebXClientBitrateCalculator {
private:
    class WebXClientBitrateData {
    public:
        WebXClientBitrateData(float bitrateMbps) :
            bitrateMbps(bitrateMbps),
            timestamp(std::chrono::high_resolution_clock::now()) {}
        virtual ~WebXClientBitrateData() {}

        float bitrateMbps;
        std::chrono::high_resolution_clock::time_point timestamp;
    };

    class WebXClientLatencyData {
    public:
        WebXClientLatencyData(float rttLatencyMs) :
            rttLatencyMs(rttLatencyMs),
            timestamp(std::chrono::high_resolution_clock::now()) {}
        virtual ~WebXClientLatencyData() {}

        float rttLatencyMs;
        std::chrono::high_resolution_clock::time_point timestamp;
    };

public:
    WebXClientBitrateCalculator() : 
        _avgBitrateMbps(WebXOptional<float>::Empty()),
        _avgRTTLatencyMs(0.0) {}
    virtual ~WebXClientBitrateCalculator() {}

    void updateLatency(uint64_t sendTimestampMs, uint64_t recvTimestampMs) {
        float rttLatencyMs = recvTimestampMs - sendTimestampMs;
        this->_latencyDataPoints.push_back(WebXClientLatencyData(rttLatencyMs));

        this->calculateAverageRTTLatency();

        // spdlog::debug("Client latency calculation: instant RTT latency = {:f}, average RTT latency = {:f}", rttLatencyMs, this->_avgRTTLatencyMs);
    }

    void updateBitrateData(uint64_t sendTimestampMs, uint64_t recvTimestampMs, uint32_t dataLength) {
        uint64_t adjustedRecvTimestampMs = recvTimestampMs - (uint64_t)this->_avgRTTLatencyMs;
        if (adjustedRecvTimestampMs > sendTimestampMs) {
            uint32_t transferTimeMs = adjustedRecvTimestampMs - sendTimestampMs;
            float bitrateMbps = (0.00762939 * dataLength) / transferTimeMs; // (b * 8 / 1024 / 1024) / (ms / 1000)
            // spdlog::debug("transfer time = {:d} for {:d} bytes => bitrate = {:f} Mb/s", transferTimeMs, dataLength, bitrateMbps);

            this->_bitrateDataPoints.push_back(WebXClientBitrateData(bitrateMbps));
        }
    }

    void resetBitrateData() {
        this->_bitrateDataPoints.clear();
        this->_avgBitrateMbps = WebXOptional<float>::Empty();
    }

    WebXOptional<float> calculateAverageBitrateMbps() {
        // Remove expired data
        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        this->_bitrateDataPoints.erase(std::remove_if(this->_bitrateDataPoints.begin(), this->_bitrateDataPoints.end(), [&now](const WebXClientBitrateData & dataPoint) {
            std::chrono::duration<double, std::milli> durationMs = now - dataPoint.timestamp;
            return durationMs.count() > BITRATE_DATA_RETENTION_TIME_MS; 
        }), this->_bitrateDataPoints.end());

        if (this->_bitrateDataPoints.size() > 1) {
            float totalBitrate = 0.0;
            for (const WebXClientBitrateData & data : this->_bitrateDataPoints) {
                totalBitrate += data.bitrateMbps;
            }

            this->_avgBitrateMbps = WebXOptional<float>::Value(totalBitrate / this->_bitrateDataPoints.size());

        } else {
            this->_avgBitrateMbps = WebXOptional<float>::Empty();
        }

        return this->_avgBitrateMbps;
    }

    WebXOptional<float> getAverageBitrateMbps() const {
        return this->_avgBitrateMbps;
    }

    float getAverageRTTLatencyMs() const {
        return this->_avgRTTLatencyMs;
    }

private:
    void calculateAverageRTTLatency() {

        // Remove expired data
        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        this->_latencyDataPoints.erase(std::remove_if(this->_latencyDataPoints.begin(), this->_latencyDataPoints.end(), [&now](const WebXClientLatencyData & dataPoint) {
            std::chrono::duration<double, std::milli> durationMs = now - dataPoint.timestamp;
            return durationMs.count() > LATENCY_DATA_RETENTION_TIME_MS; 
        }), this->_latencyDataPoints.end());
        
        if (this->_latencyDataPoints.size() > 0) {
            float totalLatencyMs = 0.0;
            for (const WebXClientLatencyData & data : this->_latencyDataPoints) {
                totalLatencyMs += data.rttLatencyMs;
            }

            this->_avgRTTLatencyMs = totalLatencyMs / this->_latencyDataPoints.size();

        } else {
            this->_avgRTTLatencyMs = 0.0;
        }
    }

private:
    const static int LATENCY_DATA_RETENTION_TIME_MS = 30000;
    const static int BITRATE_DATA_RETENTION_TIME_MS = 10000;
    std::vector<WebXClientBitrateData> _bitrateDataPoints;
    std::vector<WebXClientLatencyData> _latencyDataPoints;
    WebXOptional<float> _avgBitrateMbps;
    float _avgRTTLatencyMs; // Round-Trip Time
};


#endif /* WEBX_CLIENT_BITRATE_CALCULATOR_H */