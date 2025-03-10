#ifndef WEBX_CLIENT_BITRATE_CALCULATOR_H
#define WEBX_CLIENT_BITRATE_CALCULATOR_H

#include <chrono>
#include <spdlog/spdlog.h>
#include <models/WebXDataRate.h>

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

public:
    WebXClientBitrateCalculator() {}
    virtual ~WebXClientBitrateCalculator() {}


    void updateLatency(uint64_t sendTimestampMs, uint64_t recvTimestampMs) {
        this->_rttLatencyMs = recvTimestampMs - sendTimestampMs;
        spdlog::info("send time = {:d} recv time = {:d} RTT latency = {:d}", sendTimestampMs, recvTimestampMs, this->_rttLatencyMs);
    }

    void updateBitrateData(uint64_t sendTimestampMs, uint64_t recvTimestampMs, uint32_t dataLength) {
        uint64_t adjustedRecvTimestampMs = recvTimestampMs - this->_rttLatencyMs;
        if (adjustedRecvTimestampMs > sendTimestampMs) {
            uint32_t transferTimeMs = adjustedRecvTimestampMs - sendTimestampMs;
            float bitrateMbps = (0.00762939 * dataLength) / transferTimeMs; // (b * 8 / 1024 / 1024) / (ms / 1000)
            // spdlog::info("transfer time = {:d} for {:d} bytes => bitrate = {:f} Mb/s", transferTimeMs, dataLength, bitrateMbps);

            this->_dataPoints.push_back(WebXClientBitrateData(bitrateMbps));
        }

        this->calculateAverageBitrate();
        if (this->_averageMbps.valid) {
            spdlog::info("Average bitrate = {:f} Mb/s", this->_averageMbps.Mbps);
        }
    }

    WebXDataRate calculateAverageBitrate() {
        // Remove expired data
        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        this->_dataPoints.erase(std::remove_if(this->_dataPoints.begin(), this->_dataPoints.end(), [&now](const WebXClientBitrateData & dataPoint) {
            std::chrono::duration<double, std::milli> durationMs = now - dataPoint.timestamp;
            return durationMs.count() > DATA_RETENTION_TIME_MS; 
        }), this->_dataPoints.end());

        if (this->_dataPoints.size() > 1) {
            float totalBitrate = 0.0;
            for (const WebXClientBitrateData & data : this->_dataPoints) {
                totalBitrate += data.bitrateMbps;
            }

            this->_averageMbps = WebXDataRate(totalBitrate / this->_dataPoints.size());

        } else {
            this->_averageMbps = WebXDataRate();
        }

        return this->_averageMbps;
    }

private:
    const static int DATA_RETENTION_TIME_MS = 4000;
    std::vector<WebXClientBitrateData> _dataPoints;
    WebXDataRate _averageMbps;
    uint32_t _rttLatencyMs; // Round-Trip Time
};


#endif /* WEBX_CLIENT_BITRATE_CALCULATOR_H */