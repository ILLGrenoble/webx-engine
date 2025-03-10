#ifndef WEBX_WINDOW_QUALITY_HANDLER_H
#define WEBX_WINDOW_QUALITY_HANDLER_H

#include <vector>
#include <chrono>
#include <models/WebXSettings.h>
#include <models/WebXWindowImageTransferData.h>
#include <models/WebXQuality.h>
#include <models/WebXRectangle.h>
#include <models/WebXWindowCoverage.h>
#include <models/WebXDataRate.h>
#include <models/WebXTransferData.h>

class WebXWindowQualityHandler {
public:
    WebXWindowQualityHandler(unsigned long windowId, const WebXQuality & desiredQuality, const WebXQualitySettings & settings);
    WebXWindowQualityHandler(unsigned long windowId, const WebXQuality & desiredQuality, const WebXWindowCoverage & coverage, const WebXQualitySettings & settings);
    virtual ~WebXWindowQualityHandler();

    const WebXQuality & calculateQuality();

    void onImageTransfer(const WebXWindowImageTransferData & transferData) {
        if (transferData.status != WebXWindowImageTransferData::WebXWindowImageTransferStatus::Ignored) {
            this->_dataPoints.push_back(WebXTransferData(transferData.imageSizeKB));
            this->calculateQuality();
        }
    }

    void setWindowCoverage(const WebXWindowCoverage & coverage);

    const WebXQuality & getCurrentQuality() const {
        return this->_currentQuality;
    }

    const std::chrono::high_resolution_clock::time_point & getLastRefreshTime() const {
        return this->_lastRefreshTime;
    }

private:
    WebXDataRate calculateImageMbps();

    void setCurrentQuality(const WebXQuality & quality) {
        // If change quality empty the data points (requires one second to get new data allowing time to obtain valid stats for new level)
        if (this->_currentQuality != quality) {
            spdlog::trace("Window 0x{:x} (desired quality level {:d}) image Mb/s = {:f} quality {:s} to level {:d}", this->_windowId, this->_desiredQuality.index,  this->_imageMbps.Mbps, this->_currentQuality < quality ? "increased" : "reduced", quality.index);
            this->_currentQuality = quality;
    
            // Reset data points to give image KB/s calc time to refresh with new values
            this->_dataPoints.clear();
            this->_imageMbpsInitTime = std::chrono::high_resolution_clock::now();
        }
    }

private:
    const static int DATA_RETENTION_TIME_MS = 4000;
    const static int NO_RECENT_WINDOW_UPDATE_TIME_MS = 2000;
    const static int TIME_FOR_VALID_IMAGE_KBPS_MS = 1000;
    
    unsigned long _windowId;
    const WebXQualitySettings & _settings;
    const WebXQuality & _desiredQuality;

    WebXWindowCoverage _coverage;
    WebXQuality _coverageQuality;
    WebXQuality _imageMbpsQuality;
    WebXQuality _currentQuality;
    
    std::vector<WebXTransferData> _dataPoints;
    WebXDataRate _imageMbps;
    std::chrono::high_resolution_clock::time_point _imageMbpsInitTime;
    std::chrono::high_resolution_clock::time_point _lastRefreshTime;
};


#endif /* WEBX_WINDOW_QUALITY_HANDLER_H */