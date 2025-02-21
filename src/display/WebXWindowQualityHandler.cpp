#include "WebXWindowQualityHandler.h"
#include <algorithm>

WebXWindowQualityHandler::WebXWindowQualityHandler(unsigned long windowId) :
    _windowId(windowId),
    _coverage({.coverage = 0.0, .mouseOver = false}),
    _coverageQuality(WebXQuality::MaxQuality()),
    _imageKbpsQuality(WebXQuality::MaxQuality()),
    _currentQuality(WebXQuality::MaxQuality()),
    _desiredQuality(WebXQuality::MaxQuality()),
    _imageKbps({.valid = true, .imageKbps = 0.0}),
    _imageKbpsInitTime(std::chrono::high_resolution_clock::now()) {
}

WebXWindowQualityHandler::~WebXWindowQualityHandler() {
}

void WebXWindowQualityHandler::setWindowCoverage(const WebXRectangle::WebXRectCoverage & coverage) {
    this->_coverage = coverage;

    // quality dependent on coverage only
    // const WebXQuality & quality = webx_quality_for_image_coverage(this->_coverage.coverage);

    // quality dependent on coverage, but mouse over visible part of window improves quality
    const WebXQuality & coverageQuality = this->_coverage.mouseOver ? WebXQuality::MaxQuality() : WebXQuality::QualityForImageCoverage(this->_coverage.coverage);

    // If a window is made fully visible (eg click to bring into focus) then force the current quality to the max desired quality
    if (coverageQuality.index == WebXQuality::MAX_QUALITY_INDEX && (coverageQuality.index - this->_coverageQuality.index) > 2) {
        this->setCurrentQuality(this->_desiredQuality);
    }
    this->_coverageQuality = coverageQuality;
}

const WebXQuality & WebXWindowQualityHandler::calculateQuality(const WebXQuality & desiredQuality) {
    // Set the desired quality
    this->_desiredQuality = desiredQuality;

    // Update image KB/s
    this->_imageKbps = this->calculateImageKbps();

    // Only recalculate quality if we have a valid image KB/s value
    if (this->_imageKbps.valid) {
        // Start with coverage quality: if image KB/s > coverage quality KB/s then choose coverage quality, otherwise use desired quality
        // eg for a window with very low KB/S just keep good quality
        WebXQuality quality = this->_coverageQuality.maxKbps < this->_imageKbps.imageKbps ? this->_coverageQuality : desiredQuality;

        // If the image KB/s is too high for this quality then reduce it. If much lower then raise the quality level
        this->_imageKbpsQuality = WebXQuality::QualityForImageKbps(this->_imageKbps.imageKbps, quality, this->_currentQuality);

        // Use min of image quality and previous quality
        quality = this->_imageKbpsQuality < quality ? this->_imageKbpsQuality : quality;

        // Update the quality
        this->setCurrentQuality(quality);
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

    if (this->_dataStore.size() > 0) {
        float totalImageSizeKB = 0;
        for (auto it = this->_dataStore.begin(); it != this->_dataStore.end(); it++) {
            totalImageSizeKB += (*it).imageSizeKB;
        }
    
        std::chrono::duration<float, std::milli> durationMs = now - this->_dataStore[0].timestamp;
        if (durationMs.count() > WebXWindowQualityHandler::TIME_FOR_VALID_IMAGE_KBPS_MS) {
            float imageKbps = 1000.0 * totalImageSizeKB / durationMs.count();
            return {.valid = true, .imageKbps = imageKbps};
        }

    } else {
        // If no images have been sent after a specific time then consider the KB/s to be 0
        std::chrono::duration<float, std::milli> timeSinceImageKbpsInit = now - this->_imageKbpsInitTime;
        if (timeSinceImageKbpsInit.count() > WebXWindowQualityHandler::NO_RECENT_WINDOW_UPDATE_TIME_MS) {
            return {.valid = true, .imageKbps = 0.0};
        }
    }

    return {.valid = false};
}

