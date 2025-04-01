#ifndef WEBX_WINDOW_QUALITY_HANDLER_H
#define WEBX_WINDOW_QUALITY_HANDLER_H

#include <vector>
#include <chrono>
#include <models/WebXSettings.h>
#include <models/WebXWindowImageTransferData.h>
#include <models/WebXQuality.h>
#include <models/WebXRectangle.h>
#include <models/WebXWindowCoverage.h>
#include <models/WebXTransferData.h>
#include <utils/WebXOptional.h>

/**
 * @class WebXWindowQualityHandler
 * @brief Handles the quality calculation and management for a specific window.
 * 
 * This class is responsible for calculating and maintaining the quality level of a window
 * based on image transfer data, window coverage, and desired quality settings.
 */
class WebXWindowQualityHandler {
public:
    /**
     * @brief Constructor for initializing the handler with a window ID, desired quality, and settings.
     * @param windowId The unique identifier of the window.
     * @param desiredQuality The desired quality level for the window.
     * @param settings The quality settings to be used.
     */
    WebXWindowQualityHandler(unsigned long windowId, const WebXQuality & desiredQuality, const WebXQualitySettings & settings);

    /**
     * @brief Constructor for initializing the handler with window coverage in addition to other parameters.
     * @param windowId The unique identifier of the window.
     * @param desiredQuality The desired quality level for the window.
     * @param coverage The coverage area of the window.
     * @param settings The quality settings to be used.
     */
    WebXWindowQualityHandler(unsigned long windowId, const WebXQuality & desiredQuality, const WebXWindowCoverage & coverage, const WebXQualitySettings & settings);

    /**
     * @brief Destructor for cleaning up resources.
     */
    virtual ~WebXWindowQualityHandler();

    /**
     * @brief Calculates the current quality level based on the collected data points.
     * @return The calculated quality level.
     */
    const WebXQuality & calculateQuality();

    /**
     * @brief Processes image transfer data and updates quality calculations.
     * @param transferData The data related to the image transfer.
     */
    void onImageTransfer(const WebXWindowImageTransferData & transferData) {
        if (transferData.status != WebXWindowImageTransferData::WebXWindowImageTransferStatus::Ignored) {
            this->_dataPoints.push_back(WebXTransferData(transferData.imageSizeKB));
            this->calculateQuality();
        }
    }

    /**
     * @brief Sets the coverage area for the window.
     * @param coverage The new coverage area.
     */
    void setWindowCoverage(const WebXWindowCoverage & coverage);

    /**
     * @brief Gets the current quality level of the window.
     * @return The current quality level.
     */
    const WebXQuality & getCurrentQuality() const {
        return this->_currentQuality;
    }

    /**
     * @brief Gets the timestamp of the last quality refresh.
     * @return The timestamp of the last refresh.
     */
    const std::chrono::high_resolution_clock::time_point & getLastRefreshTime() const {
        return this->_lastRefreshTime;
    }

private:
    /**
     * @brief Calculates the image transfer rate in Mbps.
     * @return The calculated Mbps value wrapped in an optional.
     */
    WebXOptional<float> calculateImageMbps();

    /**
     * @brief Sets the current quality level and resets data points if the quality changes.
     * @param quality The new quality level to set.
     */
    void setCurrentQuality(const WebXQuality & quality) {
        // If change quality empty the data points (requires one second to get new data allowing time to obtain valid stats for new level)
        if (this->_currentQuality != quality) {
            spdlog::trace("Window 0x{:x} (desired quality level {:d}) image Mb/s = {:f} quality {:s} to level {:d}", this->_windowId, this->_desiredQuality.index, this->_imageMbps.orElse(-1.0), this->_currentQuality < quality ? "increased" : "reduced", quality.index);
            this->_currentQuality = quality;
    
            // Reset data points to give image KB/s calc time to refresh with new values
            this->_dataPoints.clear();
            this->_imageMbpsInitTime = std::chrono::high_resolution_clock::now();
        }
    }

private:
    const static int DATA_RETENTION_TIME_MS = 4000;
    const static int NO_RECENT_WINDOW_UPDATE_TIME_MS = 2000;
    const static int TIME_FOR_VALID_IMAGE_KBPS_MS = 2000;
    
    unsigned long _windowId;
    const WebXQualitySettings & _settings;
    const WebXQuality & _desiredQuality;

    WebXWindowCoverage _coverage;
    WebXQuality _coverageQuality;
    WebXQuality _imageMbpsQuality;
    WebXQuality _currentQuality;
    
    std::vector<WebXTransferData> _dataPoints;
    WebXOptional<float> _imageMbps;
    std::chrono::high_resolution_clock::time_point _imageMbpsInitTime;
    std::chrono::high_resolution_clock::time_point _lastRefreshTime;
};


#endif /* WEBX_WINDOW_QUALITY_HANDLER_H */