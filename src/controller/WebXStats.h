#ifndef WEBX_STATS_H
#define WEBX_STATS_H

#include <vector>
#include <chrono>

class WebXStats {
private:
    class WebXFrameData {
    public:
        WebXFrameData(float fps, float durationMs, float imageSizeKB) :
            timestamp(std::chrono::high_resolution_clock::now()),
            fps(fps),
            durationMs(durationMs),
            imageSizeKB(imageSizeKB) {}
        virtual ~WebXFrameData() {}

        std::chrono::high_resolution_clock::time_point timestamp;
        double fps;
        double durationMs;
        float imageSizeKB;
    };


public:
    WebXStats();
    virtual ~WebXStats();

    void updateFrameData(float fps, float durationMs, float imageSizeKB);

    float averageFps() const {
        return this->_averageFps;
    }
    
    float averageDurationMs() const {
        return this->_averageDurationMs;
    }
    
    float averageImageKbps() const {
        return this->_averageImageKbps;
    }

private:
    void removeAncientData();

private:
    const static int STATS_CALC_TIME_MS = 500;
    const static int FRAME_DATA_RETENTION_TIME_MS = 3000;

    int _storeSize;
    std::vector<WebXFrameData> _frameDataStore;

    std::chrono::high_resolution_clock::time_point _statsCalcTime;

    float _averageFps;
    float _averageDurationMs;
    float _averageImageKbps;
};


#endif /* WEBX_STATS_H */