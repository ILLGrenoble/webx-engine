#ifndef WEBX_CLIENT_WINDOW_H
#define WEBX_CLIENT_WINDOW_H

#include <X11/Xlib.h>
#include "WebXWindowQualityHandler.h"
#include <models/WebXSettings.h>
#include <models/WebXQuality.h>
#include <models/WebXRectangle.h>
#include <models/WebXWindowDamage.h>

/**
 * @class WebXClientWindow
 * @brief Represents a client-side window and manages its quality, damage, and state.
 * 
 * This class encapsulates the properties and behaviors of a client window, including
 * quality management, damage tracking, and image transfer handling.
 */
class WebXClientWindow {
public:
    /**
     * @brief Constructor for initializing a client window with coverage and rectangle.
     * @param id The unique identifier of the window.
     * @param desiredQuality The desired quality level for the window.
     * @param rectangle The rectangle defining the window's size and position.
     * @param coverage The coverage area of the window.
     * @param settings The quality settings to be used.
     */
    WebXClientWindow(Window id, const WebXQuality & desiredQuality, const WebXRectangle & rectangle, const WebXWindowCoverage & coverage, const WebXQualitySettings & settings) :
        _id(id),
        _damage(id),
        _qualityHandler(id, desiredQuality, coverage, settings),
        _windowSize(rectangle.size()),
        _imageRefreshTime(std::chrono::high_resolution_clock::now()),
        _rgbChecksum(0),
        _alphaChecksum(0) {
    }

    /**
     * @brief Constructor for initializing a client window with damage information.
     * @param id The unique identifier of the window.
     * @param desiredQuality The desired quality level for the window.
     * @param damage The damage information for the window.
     * @param settings The quality settings to be used.
     */
    WebXClientWindow(Window id, const WebXQuality & desiredQuality, const WebXWindowDamage & damage, const WebXQualitySettings & settings) :
        _id(id),
        _damage(damage),
        _qualityHandler(id, desiredQuality, settings),
        _imageRefreshTime(std::chrono::high_resolution_clock::now()),
        _rgbChecksum(0),
        _alphaChecksum(0) {
    }

    /**
     * @brief Destructor for cleaning up resources.
     */
    virtual ~WebXClientWindow() {}

    /**
     * @brief Gets the unique identifier of the window.
     * @return The window ID.
     */
    const Window & getId() const {
        return this->_id;
    }

    /**
     * @brief Gets the damage information of the window.
     * @return The window damage information.
     */
    const WebXWindowDamage & getDamage() const {
        return this->_damage;
    }

    /**
     * @brief Adds damage information to the window.
     * @param damage The damage information to be added.
     */
    void addDamage(const WebXWindowDamage & damage) {
        this->_damage += damage;
    }

    /**
     * @brief Resets the damage information of the window.
     */
    void resetDamage() {
        this->_damage.reset();
    }

    /**
     * @brief Checks if the window has any damage.
     * @return True if the window has damage, false otherwise.
     */
    bool hasDamage() const {
        return this->_damage.hasDamage();
    }

    /**
     * @brief Checks if the damage covers the full window.
     * @return True if the damage covers the full window, false otherwise.
     */
    bool isFullWindowDamage() const {
        return this->_damage.isFullWindow(this->_windowSize);
    }

    /**
     * @brief Gets the ratio of the damaged area to the total window area.
     * @return The damage area ratio.
     */
    float getDamageAreaRatio() const {
        return (1.0) * this->_damage.getDamagedArea() / this->_windowSize.area(); 
    }

    /**
     * @brief Gets the current quality of the window.
     * @return The current quality.
     */
    const WebXQuality & getCurrentQuality() const {
        return this->_qualityHandler.getCurrentQuality();
    }

    /**
     * @brief Updates the quality of the window based on the elapsed time since the last refresh.
     */
    void updateQuality() {
        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> timeSinceRefresh = now - this->_qualityHandler.getLastRefreshTime();
        if (timeSinceRefresh.count() > QUALITY_REFRESH_TIME_MS) {
            this->_qualityHandler.calculateQuality();
        }
    }

    /**
     * @brief Sets the size of the window.
     * @param size The new size of the window.
     */
    void setSize(const WebXSize & size) {
        if (this->_windowSize != size) {
            this->_windowSize = size;
        }
    }

    /**
     * @brief Sets the coverage area of the window.
     * @param coverage The new coverage area.
     */
    void setCoverage(const WebXWindowCoverage & coverage) {
        this->_qualityHandler.setWindowCoverage(coverage);
    }

    /**
     * @brief Checks if the window requires a refresh based on the reference time.
     * @param reference The reference time to compare against.
     * @return True if the window requires a refresh, false otherwise.
     */
    bool requiresRefresh(const std::chrono::high_resolution_clock::time_point & reference) {
        return this->_imageRefreshTime < reference;
    }

    /**
     * @brief Gets the RGB checksum of the window.
     * @return The RGB checksum.
     */
    uint32_t getRGBChecksum() const {
        return this->_rgbChecksum;
    }

    /**
     * @brief Sets the RGB checksum of the window.
     * @param rgbChecksum The new RGB checksum.
     */
    void setRGBChecksum(uint32_t rgbChecksum) {
        this->_rgbChecksum = rgbChecksum;
    }

    /**
     * @brief Gets the alpha checksum of the window.
     * @return The alpha checksum.
     */
    uint32_t getAlphaChecksum() const {
        return this->_alphaChecksum;
    }

    /**
     * @brief Sets the alpha checksum of the window.
     * @param alphaChecksum The new alpha checksum.
     */
    void setAlphaChecksum(uint32_t alphaChecksum) {
        this->_alphaChecksum = alphaChecksum;
    }

    /**
     * @brief Handles the image transfer data for the window.
     * @param transferData The image transfer data.
     */
    void onImageTransfer(const WebXWindowImageTransferData & transferData) {
        if (transferData.status == WebXWindowImageTransferData::WebXWindowImageTransferStatus::FullWindow) {
            this->_rgbChecksum = transferData.rgbChecksum;
            this->_alphaChecksum = transferData.alphaChecksum;
            this->_imageRefreshTime = transferData.timestamp;

        } else if (transferData.status == WebXWindowImageTransferData::WebXWindowImageTransferStatus::SubWindow) {
            this->_imageRefreshTime = transferData.timestamp;
        }

        this->_qualityHandler.onImageTransfer(transferData);
    }

private:
    const static int QUALITY_REFRESH_TIME_MS = 500;

    Window _id;
    WebXWindowDamage _damage;
    WebXWindowQualityHandler _qualityHandler;
    WebXSize _windowSize;

    std::chrono::high_resolution_clock::time_point _imageRefreshTime;

    uint32_t _rgbChecksum;
    uint32_t _alphaChecksum;
};


#endif /* WEBX_CLIENT_WINDOW_H */