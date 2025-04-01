#ifndef WEBX_CLIENT_BITRATE_CALCULATOR_H
#define WEBX_CLIENT_BITRATE_CALCULATOR_H

#include <chrono>
#include <numeric>
#include <spdlog/spdlog.h>
#include <utils/WebXOptional.h>

/**
 * @class WebXClientBitrateCalculator
 * @brief Calculates and manages bitrate and latency statistics for WebX clients.
 */
class WebXClientBitrateCalculator {
private:
    /**
     * @class WebXClientBitrateData
     * @brief Represents a single bitrate data point with its timestamp.
     */
    class WebXClientBitrateData {
    public:
        /**
         * @brief Constructs a WebXClientBitrateData object.
         * @param bitrateMbps The bitrate in Mbps.
         */
        WebXClientBitrateData(float bitrateMbps) :
            bitrateMbps(bitrateMbps),
            timestamp(std::chrono::high_resolution_clock::now()) {}
        virtual ~WebXClientBitrateData() {}

        float bitrateMbps;
        std::chrono::high_resolution_clock::time_point timestamp;
    };

    /**
     * @class WebXClientLatencyData
     * @brief Represents a single latency data point with its timestamp.
     */
    class WebXClientLatencyData {
    public:
        /**
         * @brief Constructs a WebXClientLatencyData object.
         * @param rttLatencyMs The round-trip time latency in milliseconds.
         */
        WebXClientLatencyData(float rttLatencyMs) :
            rttLatencyMs(rttLatencyMs),
            timestamp(std::chrono::high_resolution_clock::now()) {}
        virtual ~WebXClientLatencyData() {}

        float rttLatencyMs;
        std::chrono::high_resolution_clock::time_point timestamp;
    };

public:
    /**
     * @brief Constructs a WebXClientBitrateCalculator object.
     */
    WebXClientBitrateCalculator() : 
        _meanBitrateMbps(WebXOptional<float>::Empty()),
        _meanBitrateRatio(WebXOptional<float>::Empty()),
        _currentGroupImageMbps(WebXOptional<float>::Empty()),
        _meanRTTLatencyMs(WebXOptional<float>::Empty()),
        _sdRTTLatencyMs(WebXOptional<float>::Empty()) {}
    virtual ~WebXClientBitrateCalculator() {}

    /**
     * @brief Updates latency statistics based on send and receive timestamps.
     * @param sendTimestampMs The send timestamp in milliseconds.
     * @param recvTimestampMs The receive timestamp in milliseconds.
     */
    void updateLatency(uint64_t sendTimestampMs, uint64_t recvTimestampMs) {
        float rttLatencyMs = recvTimestampMs - sendTimestampMs;
        this->_latencyDataPoints.push_back(WebXClientLatencyData(rttLatencyMs));

        this->calculateRTTLatencyStats();

        // spdlog::debug("Client latency calculation: instant RTT latency = {:f}, mean RTT latency = {:f} sd = {:f}", rttLatencyMs, this->_meanRTTLatencyMs, this->_sdRTTLatencyMs);
    }

    /**
     * @brief Updates bitrate statistics based on data transfer information.
     * @param sendTimestampMs The send timestamp in milliseconds.
     * @param recvTimestampMs The receive timestamp in milliseconds.
     * @param dataLength The length of the data transferred in bytes.
     */
    void updateBitrateData(uint64_t sendTimestampMs, uint64_t recvTimestampMs, uint32_t dataLength) {
        if (this->_meanRTTLatencyMs.hasValue()) {
            // Be pessimistic on bandwidth (use lower value latency)
            uint64_t adjustedRecvTimestampMs = recvTimestampMs - (uint64_t)(this->_meanRTTLatencyMs.value() - this->_sdRTTLatencyMs.value());
            if (adjustedRecvTimestampMs > sendTimestampMs) {
                uint32_t transferTimeMs = adjustedRecvTimestampMs - sendTimestampMs;
                float bitrateMbps = (0.00762939 * dataLength) / transferTimeMs; // (B * 8 / 1024 / 1024) / (ms / 1000)

                // spdlog::debug("transfer time = {:d} for {:d} bytes => bitrate = {:.2f} Mb/s (imageMbps = {:.2f}, ratio = {:.2f})", transferTimeMs, dataLength, bitrateMbps);

                this->_bitrateDataPoints.push_back(WebXClientBitrateData(bitrateMbps));
            }
        }

        this->calculateAverageBitrateData();
    }

    /**
     * @brief Resets bitrate data and sets the current group image bitrate.
     * @param currentGroupImageMbps The current group image bitrate in Mbps.
     */
    void resetBitrateData(const WebXOptional<float> & currentGroupImageMbps) {
        this->_bitrateDataPoints.clear();
        this->_meanBitrateMbps = WebXOptional<float>::Empty();
        this->_meanBitrateRatio = WebXOptional<float>::Empty();
        this->_currentGroupImageMbps = currentGroupImageMbps;
    }

    /**
     * @brief Calculates average bitrate statistics from the collected data points.
     */
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
            this->_currentGroupImageMbps.orElse(-1.0), 
            this->_meanBitrateMbps.orElse(-1.0),
            this->_bitrateDataPoints.size(),
            this->_meanBitrateRatio.orElse(-1.0),
            this->_meanRTTLatencyMs.orElse(-1.0),
            this->_sdRTTLatencyMs.orElse(-1.0));
    }

    /**
     * @brief Gets the mean bitrate in Mbps.
     * @return The mean bitrate as a WebXOptional<float>.
     */
    const WebXOptional<float> & getMeanBitrateMbps() const {
        return this->_meanBitrateMbps;
    }

    /**
     * @brief Gets the mean bitrate ratio.
     * @return The mean bitrate ratio as a WebXOptional<float>.
     */
    const WebXOptional<float> & getMeanBitrateRatio() const {
        return this->_meanBitrateRatio;
    }
    
    /**
     * @brief Sets the current group image bitrate.
     * @param currentGroupImageMbps The current group image bitrate in Mbps.
     */
    void setCurrentGroupImageMbps(const WebXOptional<float> & currentGroupImageMbps) {
        this->_currentGroupImageMbps = currentGroupImageMbps;
    }

    /**
     * @brief Gets the mean round-trip time (RTT) latency in milliseconds.
     * @return The mean RTT latency as a WebXOptional<float>.
     */
    const WebXOptional<float> & getMeanRTTLatencyMs() const {
        return this->_meanRTTLatencyMs;
    }

private:
    /**
     * @brief Calculates RTT latency statistics (mean and standard deviation).
     */
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
    
            this->_meanRTTLatencyMs = WebXOptional<float>::Value(mean);
            this->_sdRTTLatencyMs = WebXOptional<float>::Value(std::sqrt(sumSquaredDiffs / this->_latencyDataPoints.size()));
    

        } else {
            this->_meanRTTLatencyMs = WebXOptional<float>::Empty();
            this->_sdRTTLatencyMs = WebXOptional<float>::Empty();
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
    WebXOptional<float> _meanRTTLatencyMs;
    WebXOptional<float> _sdRTTLatencyMs;
};

#endif /* WEBX_CLIENT_BITRATE_CALCULATOR_H */