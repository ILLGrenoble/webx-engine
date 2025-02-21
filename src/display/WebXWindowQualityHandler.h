#ifndef WEBX_WINDOW_QUALITY_HANDLER_H
#define WEBX_WINDOW_QUALITY_HANDLER_H

#include <vector>
#include <chrono>
#include <utils/WebXQuality.h>
#include <utils/WebXRectangle.h>

class WebXWindowQualityHandler {
private:
    class WebXWindowQualityData {
    public:
        WebXWindowQualityData(float imageSizeKB) :
            timestamp(std::chrono::high_resolution_clock::now()),
            imageSizeKB(imageSizeKB) {}
        virtual ~WebXWindowQualityData() {}

        std::chrono::high_resolution_clock::time_point timestamp;
        float imageSizeKB;
    };

    struct WebXWindowImageKbs {
        bool valid;
        float imageKbps;
    };

public:
    WebXWindowQualityHandler(unsigned long windowId);
    virtual ~WebXWindowQualityHandler();

    void onImageDataSent(float imageSizeKB) {
        WebXWindowQualityData data = WebXWindowQualityData(imageSizeKB);
        this->_dataStore.push_back(data);
    }

    void setWindowCoverage(const WebXRectangle::WebXRectCoverage & coverage);

    const WebXQuality & calculateQuality(const WebXQuality & desiredQuality);

    const WebXQuality & getCurrentQuality() const {
        return this->_currentQuality;
    }

private:
    WebXWindowImageKbs calculateImageKbps();
    void setCurrentQuality(const WebXQuality & quality) {
        // If change quality empty the dataStore (requires one second to get new data allowing time to obtain valid stats for new level)
        if (this->_currentQuality != quality) {
            spdlog::debug("Window 0x{:x} image KB/s = {:f} quality {:s} to level {:d}", this->_windowId, this->_imageKbps.imageKbps, this->_currentQuality < quality ? "increased" : "reduced", quality.index);
            this->_currentQuality = quality;
    
            // Reset data store to give image KB/s calc time to refresh with new values
            this->_dataStore.clear();
            this->_imageKbpsInitTime = std::chrono::high_resolution_clock::now();
        }
    }

private:
    const static int DATA_RETENTION_TIME_MS = 4000;
    const static int NO_RECENT_WINDOW_UPDATE_TIME_MS = 2000;
    const static int TIME_FOR_VALID_IMAGE_KBPS_MS = 1000;
    
    unsigned long _windowId;

    WebXRectangle::WebXRectCoverage _coverage;
    WebXQuality _coverageQuality;
    WebXQuality _imageKbpsQuality;
    WebXQuality _currentQuality;
    WebXQuality _desiredQuality;
    
    std::vector<WebXWindowQualityData> _dataStore;
    WebXWindowImageKbs _imageKbps;
    std::chrono::high_resolution_clock::time_point _imageKbpsInitTime;
};


#endif /* WEBX_WINDOW_QUALITY_HANDLER_H */