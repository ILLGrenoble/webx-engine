#ifndef WEBX_STATS_H
#define WEBX_STATS_H

#include <vector>
#include <chrono>

/**
 * @class WebXStats
 * @brief Collects and calculates performance statistics for the WebX engine.
 */
class WebXStats {
private:
    /**
     * @class WebXFrameData
     * @brief Stores data for a single frame, including FPS, duration, and image size.
     */
    class WebXFrameData {
    public:
        /**
         * @brief Constructs a WebXFrameData instance.
         * @param fps Frames per second for the frame.
         * @param durationMs Duration of the frame in milliseconds.
         * @param imageSizeKB Size of the frame image in kilobytes.
         */
        WebXFrameData(float fps, float durationMs, float imageSizeKB) :
            timestamp(std::chrono::high_resolution_clock::now()),
            fps(fps),
            durationMs(durationMs),
            imageSizeKB(imageSizeKB) {}

        /**
         * @brief Destructor for WebXFrameData.
         */
        virtual ~WebXFrameData() {}

        std::chrono::high_resolution_clock::time_point timestamp;
        double fps;
        double durationMs;
        float imageSizeKB;
    };

public:
    /**
     * @brief Constructs a WebXStats instance.
     */
    WebXStats();

    /**
     * @brief Destructor for WebXStats.
     */
    virtual ~WebXStats();

    /**
     * @brief Updates the frame data with new statistics.
     * @param fps Frames per second.
     * @param durationMs Duration of the frame in milliseconds.
     * @param imageSizeKB Size of the frame image in kilobytes.
     */
    void updateFrameData(float fps, float durationMs, float imageSizeKB);

    /**
     * @brief Gets the average frames per second.
     * @return Average FPS.
     */
    float averageFps() const {
        return this->_averageFps;
    }

    /**
     * @brief Gets the average frame duration in milliseconds.
     * @return Average duration in milliseconds.
     */
    float averageDurationMs() const {
        return this->_averageDurationMs;
    }

    /**
     * @brief Gets the average image size in megabits per second.
     * @return Average image size in Mbps.
     */
    float averageImageMbps() const {
        return this->_averageImageMbps;
    }

private:
    /**
     * @brief Removes outdated frame data from the store.
     */
    void removeAncientData();

private:
    const static int STATS_CALC_TIME_MS = 500;
    const static int FRAME_DATA_RETENTION_TIME_MS = 3000;

    int _storeSize;
    std::vector<WebXFrameData> _frameDataStore;

    std::chrono::high_resolution_clock::time_point _statsCalcTime;

    float _averageFps;
    float _averageDurationMs;
    float _averageImageMbps;
};

#endif /* WEBX_STATS_H */