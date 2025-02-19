#include "WebXWindowQualityHandler.h"
#include <utils/WebXQualityHelper.h>
#include <algorithm>

WebXWindowQualityHandler::WebXWindowQualityHandler(unsigned long windowId) :
    _windowId(windowId),
    _coverage({.coverage = 0.0, .mouseOver = false}),
    _coverageQuality(webx_quality_max()),
    _imageKbpsQuality(webx_quality_max()),
    _currentQuality(webx_quality_max()),
    _imageKbps({.valid = false, .imageKbps = 0.0}) {
}

WebXWindowQualityHandler::~WebXWindowQualityHandler() {
}

void WebXWindowQualityHandler::setWindowCoverage(const WebXRectangle::WebXRectCoverage & coverage) {
    this->_coverage = coverage;

    // quality dependent on coverage only
    // const WebXQuality & quality = webx_quality_for_image_coverage(this->_coverage.coverage);

    // quality dependent on coverage, but mouse over visible part of window improves quality
    this->_coverageQuality = this->_coverage.mouseOver ? webx_quality_max() : webx_quality_for_image_coverage(this->_coverage.coverage);

    // spdlog::info("window 0x{:01x} quality index {:d}", window->getX11Window(), quality.index);
}

const WebXQuality & WebXWindowQualityHandler::calculateQuality(const WebXQuality & desiredQuality) {

    // Start with min quality between requested and calculated quality
    WebXQuality quality = this->_coverageQuality.index < desiredQuality.index ? this->_coverageQuality : desiredQuality;

    // Update image KB/s
    this->_imageKbps = this->calculateImageKbps();

    if (this->_imageKbps.valid) {
        // Update the imageKbps quality
        this->_imageKbpsQuality = webx_quality_for_image_kbps(this->_imageKbps.imageKbps, quality, this->_currentQuality);
    }

    // Use min of image quality and previous quality
    quality = this->_imageKbpsQuality.index < quality.index ? this->_imageKbpsQuality : quality;

    // If change quality empty the dataStore (requires one second to get new data allowing time to obtain valid stats for new level)
    if (quality.index != this->_currentQuality.index) {
        spdlog::debug("Window 0x{:x} quality {:s} to level {:d}", this->_windowId, this->_currentQuality.index < quality.index ? "increased" : "reduced", quality.index);
        this->_currentQuality = quality;

        // Reset data store to give image KB/s calc time to refresh with new values
        this->_dataStore.clear();
    }

    return this->_currentQuality;
}

WebXWindowQualityHandler::WebXWindowImageKbs WebXWindowQualityHandler::calculateImageKbps() {

    // Remove data points that are too old
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    this->_dataStore.erase(std::remove_if(this->_dataStore.begin(), this->_dataStore.end(), [now](const WebXWindowQualityData dataPoint) {
        std::chrono::duration<double, std::milli> durationMs = now - dataPoint.timestamp;
        return durationMs.count() > WebXWindowQualityHandler::DATA_RETENTION_TIME_MS; 
    }), this->_dataStore.end());

    if (this->_dataStore.size() > 1) {
        float totalImageSizeKB = 0;
        for (auto it = this->_dataStore.begin(); it != this->_dataStore.end(); it++) {
            totalImageSizeKB += (*it).imageSizeKB;
        }
    
        std::chrono::duration<float, std::milli> durationMs = this->_dataStore[this->_dataStore.size() - 1].timestamp - this->_dataStore[0].timestamp;
        if (durationMs.count() > WebXWindowQualityHandler::TIME_FOR_VALID_IMAGE_KBPS_MS) {
            float imageKbps = 1000.0 * totalImageSizeKB / durationMs.count();
            return {.valid = true, .imageKbps = imageKbps};
        }
    }

    return {.valid = false};
}

