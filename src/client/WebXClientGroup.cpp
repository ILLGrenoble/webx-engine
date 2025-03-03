#include "WebXClientGroup.h"
#include "WebXClient.h"
#include <models/WebXWindowVisibility.h>

WebXClientGroup::WebXClientGroup(const WebXSettings & settings, const WebXQuality & quality) :
    _settings(settings),
    _quality(quality),
    _clientIndexMask(0) {

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
}

void WebXClientGroup::handleWindowDamage(std::function<WebXResult<WebXWindowImageTransferData>(const std::unique_ptr<WebXClientWindow> & window, uint64_t clientIndexMask)> damageHandlerFunc) {

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

            } else {
                spdlog::error("Error handling damage for window 0x{:0x} with desired quality level {:d}: {:s}", window->getId(), this->_quality.index, result.error());
            }

            // Remove the damage from the window
            window->resetDamage();
        }
    }
}
