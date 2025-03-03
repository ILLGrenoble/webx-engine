#include "WebXStats.h"
#include <spdlog/spdlog.h>

WebXStats::WebXStats() :
    _statsCalcTime(std::chrono::high_resolution_clock::now()) {
}

WebXStats::~WebXStats() {
}

void WebXStats::updateFrameData(float fps, float durationMs, float imageSizeKB) {
    WebXFrameData currentFrameData = WebXFrameData(fps, durationMs, imageSizeKB);
    this->_frameDataStore.push_back(currentFrameData);

    this->removeAncientData();

    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float, std::milli> timeSinceCalc = now - this->_statsCalcTime;

    if (timeSinceCalc.count() >= WebXStats::STATS_CALC_TIME_MS && this->_frameDataStore.size() > 1) {
        float totalFps = 0;
        double totalDurationMs = 0;
        float totalImageSizeKB = 0;
        for (const WebXFrameData & frameData : this->_frameDataStore) {
            totalFps += frameData.fps;
            totalDurationMs += frameData.durationMs;
            totalImageSizeKB += frameData.imageSizeKB;
        }
        this->_averageFps = totalFps / this->_frameDataStore.size();
        this->_averageDurationMs = totalDurationMs / this->_frameDataStore.size();
        std::chrono::duration<float, std::milli> durationMs = currentFrameData.timestamp - this->_frameDataStore[0].timestamp;
        this->_averageImageKbps = 1000.0 * totalImageSizeKB / durationMs.count();

        spdlog::trace("Average FPS = {:f}, average frame duration = {:f}ms, average image data rate = {:f} KB/s", this->_averageFps, this->_averageDurationMs, this->_averageImageKbps);
    
        this->_statsCalcTime = now;
    }
}

void WebXStats::removeAncientData() {
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();

    // Remove ancient frame data
    this->_frameDataStore.erase(std::remove_if(this->_frameDataStore.begin(), this->_frameDataStore.end(), [&now](const WebXFrameData & dataPoint) { 
        std::chrono::duration<double, std::milli> durationMs = now - dataPoint.timestamp;
        return durationMs.count() > WebXStats::FRAME_DATA_RETENTION_TIME_MS; 
    }), this->_frameDataStore.end());
}

