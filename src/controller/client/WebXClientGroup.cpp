#include "WebXClientGroup.h"
#include "WebXClient.h"
#include <models/WebXWindowVisibility.h>

WebXClientGroup::WebXClientGroup(const WebXSettings & settings, const WebXQuality & quality) :
    _settings(settings),
    _quality(quality),
    _clientIndexMask(0),
    _lastQualityVerificationTime(std::chrono::high_resolution_clock::now()) {

}

WebXClientGroup::~WebXClientGroup() {

}

void WebXClientGroup::updateVisibleWindows(const std::vector<const WebXWindowVisibility *> & windowVisibilities) {
    // Remove windows that no longer exist
    this->_windows.erase(std::remove_if(this->_windows.begin(), this->_windows.end(), [&windowVisibilities](const std::unique_ptr<WebXClientWindow> & window) {
        auto it = std::find_if(windowVisibilities.begin(), windowVisibilities.end(), [&window](const WebXWindowVisibility * windowVisibility) {
            return windowVisibility->getX11Window() == window->getId();
        });

        return it == windowVisibilities.end();
    }), this->_windows.end());

    // Add or update windows
    for (const WebXWindowVisibility * windowVisibility : windowVisibilities) {
        auto it = std::find_if(this->_windows.begin(), this->_windows.end(), [&windowVisibility](const std::unique_ptr<WebXClientWindow> & window) {
            return windowVisibility->getX11Window() == window->getId();
        });

        if (it == this->_windows.end()) {
            this->_windows.push_back(std::unique_ptr<WebXClientWindow>(new WebXClientWindow(windowVisibility->getX11Window(), this->_quality, windowVisibility->getRectangle(), windowVisibility->getCoverage(), this->_settings.quality)));
        
        } else {
            std::unique_ptr<WebXClientWindow> & window = *it;
            window->setSize(windowVisibility->getRectangle().size());
            window->setCoverage(windowVisibility->getCoverage());
        }
    }

    // Update quality calc for all windows that haven't been refreshed for a while
    for (std::unique_ptr<WebXClientWindow> & window : this->_windows) {
        window->updateQuality();
    }
}

void WebXClientGroup::handleWindowDamage(std::function<WebXResult<WebXWindowImageTransferData>(const std::unique_ptr<WebXClientWindow> & window, uint64_t clientIndexMask)> damageHandlerFunc) {

    float totalImageSizeKB = 0.0;

    // Handle all windows that have damage and need to be refreshed
    for (std::unique_ptr<WebXClientWindow> & window : this->_windows) {

        // Get the current calculated window quality
        const WebXQuality & calculatedQuality = window->getCurrentQuality();

        // Get a reference time: any windows with refresh times smaller than this need to be updated
        std::chrono::high_resolution_clock::time_point reference = std::chrono::high_resolution_clock::now() - std::chrono::microseconds(calculatedQuality.imageUpdateTimeUs);

        if (window->hasDamage() && window->requiresRefresh(reference)) {
            // Handle the image grab and transfer with quality information
            WebXResult<WebXWindowImageTransferData> result = damageHandlerFunc(window, this->_clientIndexMask);
            if (result.ok()) {
                // If image grab and transfer ok then update the client window data
                const WebXWindowImageTransferData & transferData = result.data();
                window->onImageTransfer(transferData);

                // Update total amount of data transferred
                totalImageSizeKB += transferData.imageSizeKB;

            } else {
                spdlog::error("Error handling damage for window 0x{:0x} with desired quality level {:d}: {:s}", window->getId(), this->_quality.index, result.error());
            }

            // Remove the damage from the window
            window->resetDamage();
        }
    }

    this->_transferDataPoints.push_back(WebXTransferData(totalImageSizeKB));
}

void WebXClientGroup::performQualityVerification() {
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float, std::milli> durationMs = now - this->_lastQualityVerificationTime;
    if (durationMs.count() >= QUALITY_VERIFICATION_PERIOD_MS) {
        // Update the image data transfer calculation
        this->calculateImageMbps();

        // Update each client bitrate calculation
        for (auto client : this->_clients) {
            WebXDataRate clientDataRate = client->calculateAverageBitrate();
            if (clientDataRate.valid) {
                spdlog::info("Client {:08x}: Sending data at {:.2f} Mbps to client that receives at {:.2f} Mbps with {:.0f} ms latency", client->getId(), this->_averageImageMbps, clientDataRate.Mbps, client->getAverageRTTLatencyMs());
            }
        }

        this->_lastQualityVerificationTime = now;
    }
}

void WebXClientGroup::calculateImageMbps() {
    // Remove data points that are too old
    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    this->_transferDataPoints.erase(std::remove_if(this->_transferDataPoints.begin(), this->_transferDataPoints.end(), [&now](const WebXTransferData & dataPoint) {
        std::chrono::duration<double, std::milli> durationMs = now - dataPoint.timestamp;
        return durationMs.count() > WebXClientGroup::BITRATE_DATA_RETENTION_TIME_MS; 
    }), this->_transferDataPoints.end());

    if (this->_transferDataPoints.size() > 0) {
        float totalImageSizeKB = 0;
        for (const WebXTransferData & transferData : this->_transferDataPoints) {
            totalImageSizeKB += transferData.sizeKB;
        }
    
        std::chrono::duration<float, std::milli> durationMs = now - this->_transferDataPoints[0].timestamp;
        if (durationMs.count() > WebXClientGroup::TIME_FOR_VALID_IMAGE_KBPS_MS) {
            this->_averageImageMbps = 7.8125 * totalImageSizeKB / durationMs.count(); // (KB * 8 / 1024) / (ms / 1000)
        
        } else {
            // Consider it to be 0 if not enough time to make a coherent calculation
            this->_averageImageMbps = 0.0;
        }

    } else {
        // If no images have been sent after a specific time then consider the KB/s to be 0
        this->_averageImageMbps = 0.0;
    }
}
