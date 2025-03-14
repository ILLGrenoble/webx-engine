#ifndef WEBX_CLIENT_BITRATE_CALCULATOR_H
#define WEBX_CLIENT_BITRATE_CALCULATOR_H

#include <chrono>
#include <numeric>
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
        _meanBitrateMbps(WebXOptional<float>::Empty()),
        _meanBitrateRatio(WebXOptional<float>::Empty()),
        _currentGroupImageMbps(WebXOptional<float>::Empty()),
        _meanRTTLatencyMs(0.0),
        _sdRTTLatencyMs(0.0) {}
    virtual ~WebXClientBitrateCalculator() {}

    void updateLatency(uint64_t sendTimestampMs, uint64_t recvTimestampMs) {
        float rttLatencyMs = recvTimestampMs - sendTimestampMs;
        this->_latencyDataPoints.push_back(WebXClientLatencyData(rttLatencyMs));

        this->calculateRTTLatencyStats();

        // spdlog::debug("Client latency calculation: instant RTT latency = {:f}, mean RTT latency = {:f} sd = {:f}", rttLatencyMs, this->_meanRTTLatencyMs, this->_sdRTTLatencyMs);
    }

    void updateBitrateData(uint64_t sendTimestampMs, uint64_t recvTimestampMs, uint32_t dataLength) {
        // Be pessimistic on bandwidth (use lower value latency)
        uint64_t adjustedRecvTimestampMs = recvTimestampMs - (uint64_t)(this->_meanRTTLatencyMs - this->_sdRTTLatencyMs);
        if (adjustedRecvTimestampMs > sendTimestampMs) {
            uint32_t transferTimeMs = adjustedRecvTimestampMs - sendTimestampMs;
            float bitrateMbps = (0.00762939 * dataLength) / transferTimeMs; // (B * 8 / 1024 / 1024) / (ms / 1000)

            // spdlog::debug("transfer time = {:d} for {:d} bytes => bitrate = {:.2f} Mb/s (imageMbps = {:.2f}, ratio = {:.2f})", transferTimeMs, dataLength, bitrateMbps);

            this->_bitrateDataPoints.push_back(WebXClientBitrateData(bitrateMbps));
        }

        this->calculateAverageBitrateData();
    }

    void resetBitrateData(const WebXOptional<float> & currentGroupImageMbps) {
        this->_bitrateDataPoints.clear();
        this->_meanBitrateMbps = WebXOptional<float>::Empty();
        this->_meanBitrateRatio = WebXOptional<float>::Empty();
        this->_currentGroupImageMbps = currentGroupImageMbps;
    }

    void calculateAverageBitrateData() {
        // Remove expired data
        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        this->_bitrateDataPoints.erase(std::remove_if(this->_bitrateDataPoints.begin(), this->_bitrateDataPoints.end(), [&now](const WebXClientBitrateData & dataPoint) {
            std::chrono::duration<double, std::milli> durationMs = now - dataPoint.timestamp;
            return durationMs.count() > BITRATE_DATA_RETENTION_TIME_MS; 
        }), this->_bitrateDataPoints.end());

        if (this->_bitrateDataPoints.size() > 1) {
            std::chrono::duration<float, std::milli> durationMs = now - this->_bitrateDataPoints[0].timestamp;
            if (durationMs.count() > WebXClientBitrateCalculator::TIME_FOR_VALID_BITRATE_CALCULATION) {
                // Calculate mean bitrate
                float meanBitrateMbps = std::accumulate(this->_bitrateDataPoints.begin(), this->_bitrateDataPoints.end(), 0.0f, [](float sum, const WebXClientBitrateData & data) { 
                    return sum + data.bitrateMbps; 
                }) / this->_bitrateDataPoints.size();

                this->_meanBitrateMbps = WebXOptional<float>::Value(meanBitrateMbps);
                this->_meanBitrateRatio = this->_currentGroupImageMbps.hasValue() ? WebXOptional<float>::Value(this->_currentGroupImageMbps.value() / meanBitrateMbps) : WebXOptional<float>::Empty();

            } else {
                this->_meanBitrateMbps = WebXOptional<float>::Empty();
                this->_meanBitrateRatio = WebXOptional<float>::Empty();
            }

        } else {
            this->_meanBitrateMbps = WebXOptional<float>::Empty();
        }

        spdlog::trace("Bitrate data: image bitrate = {:2f} Mbps, bandwidth = {:.2f} Mbps ({:d} pts), ratio = {:.2f}, latency = {:.0f} ms (sd = {:.1f})", 
            _currentGroupImageMbps.hasValue() ? _currentGroupImageMbps.value() : -1.0, 
            _meanBitrateMbps.hasValue() ? _meanBitrateMbps.value() : -1.0,
            _bitrateDataPoints.size(),
            _meanBitrateRatio.hasValue() ? _meanBitrateRatio.value() : -1.0,
            this->_meanRTTLatencyMs,
            this->_sdRTTLatencyMs);
    }

    const WebXOptional<float> & getMeanBitrateMbps() const {
        return this->_meanBitrateMbps;
    }

    const WebXOptional<float> & getMeanBitrateRatio() const {
        return this->_meanBitrateRatio;
    }
    
    void setCurrentGroupImageMbps(const WebXOptional<float> & currentGroupImageMbps) {
        this->_currentGroupImageMbps = currentGroupImageMbps;
    }

    float getMeanRTTLatencyMs() const {
        return this->_meanRTTLatencyMs;
    }

private:
    void calculateRTTLatencyStats() {

        // Remove expired data
        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        this->_latencyDataPoints.erase(std::remove_if(this->_latencyDataPoints.begin(), this->_latencyDataPoints.end(), [&now](const WebXClientLatencyData & dataPoint) {
            std::chrono::duration<double, std::milli> durationMs = now - dataPoint.timestamp;
            return durationMs.count() > LATENCY_DATA_RETENTION_TIME_MS; 
        }), this->_latencyDataPoints.end());
        
        if (this->_latencyDataPoints.size() > 0) {
            float mean = std::accumulate(this->_latencyDataPoints.begin(), this->_latencyDataPoints.end(), 0.0f, [](float sum, const WebXClientLatencyData & data) { 
                return sum + data.rttLatencyMs; 
            }) / this->_latencyDataPoints.size();

            float sumSquaredDiffs = std::accumulate(this->_latencyDataPoints.begin(), this->_latencyDataPoints.end(), 0.0f, [mean](float sum, const WebXClientLatencyData & data) {
                float diff = data.rttLatencyMs - mean;
                return sum + diff * diff;
            });
    
            this->_meanRTTLatencyMs = mean;
            this->_sdRTTLatencyMs = std::sqrt(sumSquaredDiffs / this->_latencyDataPoints.size());
    

        } else {
            this->_meanRTTLatencyMs = 0.0;
            this->_sdRTTLatencyMs = 0.0;
        }
    }

private:
    const static int BITRATE_DATA_RETENTION_TIME_MS = 4000;
    const static int TIME_FOR_VALID_BITRATE_CALCULATION = 2000;
    const static int LATENCY_DATA_RETENTION_TIME_MS = 10000;

    std::vector<WebXClientBitrateData> _bitrateDataPoints;
    std::vector<WebXClientLatencyData> _latencyDataPoints;

    WebXOptional<float> _meanBitrateMbps;
    WebXOptional<float> _meanBitrateRatio;
    WebXOptional<float> _currentGroupImageMbps;
    
    float _meanRTTLatencyMs;
    float _sdRTTLatencyMs;
};


#endif /* WEBX_CLIENT_BITRATE_CALCULATOR_H */