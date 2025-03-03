#include "WebXWindowQualityHandler.h"
#include <algorithm>

WebXWindowQualityHandler::WebXWindowQualityHandler(unsigned long windowId, const WebXQuality & desiredQuality, const WebXQualitySettings & settings) :
    _windowId(windowId),
    _desiredQuality(desiredQuality),
    _settings(settings),
    _coverageQuality(WebXQuality::MaxQuality()),
    _imageKbpsQuality(WebXQuality::MaxQuality()),
    _currentQuality(WebXQuality::MaxQuality()),
    _imageKbps({.valid = true, .kbPerSecond = 0.0}),
    _imageKbpsInitTime(std::chrono::high_resolution_clock::now()) {
}

WebXWindowQualityHandler::WebXWindowQualityHandler(unsigned long windowId, const WebXQuality & desiredQuality, const WebXWindowCoverage & coverage, const WebXQualitySettings & settings) :
    _windowId(windowId),
    _desiredQuality(desiredQuality),
    _settings(settings),
    _coverageQuality(WebXQuality::MaxQuality()),
    _imageKbpsQuality(WebXQuality::MaxQuality()),
    _currentQuality(WebXQuality::MaxQuality()),
    _imageKbps({.valid = true, .kbPerSecond = 0.0}),
    _imageKbpsInitTime(std::chrono::high_resolution_clock::now()) {
    
    this->setWindowCoverage(coverage);
}

WebXWindowQualityHandler::~WebXWindowQualityHandler() {
}

void WebXWindowQualityHandler::setWindowCoverage(const WebXWindowCoverage & coverage) {
    if (this->_coverage != coverage) {
        this->_coverage = coverage;

        if (this->_settings.coverageQualityFunc != WebXQualitySettings::CoverageQualityFunc::Disabled) {
            // Use either linear or quadratic function to calculate the quality for the coverage
            const WebXQuality qualityForImageCoverage = this->_settings.coverageQualityFunc != WebXQualitySettings::CoverageQualityFunc::Linear ? 
                WebXQuality::QualityForImageCoverageLinear(this->_coverage.coverage) :
                WebXQuality::QualityForImageCoverageQuadratic(this->_coverage.coverage);
    
            // Take into account if the mouse if over a visible part of the window (to improve quality)
            const WebXQuality & coverageQuality = this->_settings.increaseQualityOnMouseOver ?
                this->_coverage.mouseOver ? WebXQuality::MaxQuality() : qualityForImageCoverage : 
                qualityForImageCoverage;
    
            // If a window is made fully visible (eg click to bring into focus) then force the current quality to the max desired quality
            if (coverageQuality.index == WebXQuality::MAX_QUALITY_INDEX && (coverageQuality.index - this->_coverageQuality.index) > 2) {
                // const WebXQuality & fastImprovedQuality = WebXQuality::QualityForIndex(std::min(this->_currentQuality.index + 2, this->_desiredQuality.index));
                // this->setCurrentQuality(fastImprovedQuality);

                this->setCurrentQuality(this->_desiredQuality);
            }
    
            this->_coverageQuality = coverageQuality;
        }

        this->calculateQuality();
    }
}

const WebXQuality & WebXWindowQualityHandler::calculateQuality() {
    // Update image KB/s
    this->_imageKbps = this->calculateImageKbps();

    if (this->_settings.limitQualityByDataRate) {
        // Only recalculate quality if we have a valid image KB/s value
        if (this->_imageKbps.valid) {
            // Start with coverage quality: if image KB/s > coverage quality KB/s then choose coverage quality, otherwise use desired quality
            // eg for a window with very low KB/S just keep good quality
            WebXQuality quality = this->_coverageQuality.maxKbps < this->_imageKbps.kbPerSecond ? this->_coverageQuality : this->_desiredQuality;

            // If the image KB/s is too high for this quality then reduce it. If much lower then raise the quality level
            this->_imageKbpsQuality = WebXQuality::QualityForImageKbps(this->_imageKbps.kbPerSecond, quality, this->_currentQuality);

            // Use min of image quality and previous quality
            quality = this->_imageKbpsQuality < quality ? this->_imageKbpsQuality : quality;

            // Update the quality
            this->setCurrentQuality(quality);
        }

    } else {
        // Update the quality
        this->setCurrentQuality(this->_desiredQuality);
    }

    return this->_currentQuality;
}

WebXWindowQualityHandler::WebXDataRate WebXWindowQualityHandler::calculateImageKbps() {

    // Remove data points that are too old
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    this->_dataPoints.erase(std::remove_if(this->_dataPoints.begin(), this->_dataPoints.end(), [&now](const WebXWindowQualityData & dataPoint) {
        std::chrono::duration<double, std::milli> durationMs = now - dataPoint.timestamp;
        return durationMs.count() > WebXWindowQualityHandler::DATA_RETENTION_TIME_MS; 
    }), this->_dataPoints.end());

    if (this->_dataPoints.size() > 0) {
        float totalImageSizeKB = 0;
        for (const WebXWindowQualityData & transferData : this->_dataPoints) {
            totalImageSizeKB += transferData.imageSizeKB;
        }
    
        std::chrono::duration<float, std::milli> durationMs = now - this->_dataPoints[0].timestamp;
        if (durationMs.count() > WebXWindowQualityHandler::TIME_FOR_VALID_IMAGE_KBPS_MS) {
            float imageKbps = 1000.0 * totalImageSizeKB / durationMs.count();
            return {.valid = true, .kbPerSecond = imageKbps};
        }

    } else {
        // If no images have been sent after a specific time then consider the KB/s to be 0
        std::chrono::duration<float, std::milli> timeSinceImageKbpsInit = now - this->_imageKbpsInitTime;
        if (timeSinceImageKbpsInit.count() > WebXWindowQualityHandler::NO_RECENT_WINDOW_UPDATE_TIME_MS) {
            return {.valid = true, .kbPerSecond = 0.0};
        }
    }

    return {.valid = false};
}

