#ifndef WEBX_CLIENT_WINDOW_H
#define WEBX_CLIENT_WINDOW_H

#include <X11/Xlib.h>
#include "WebXWindowQualityHandler.h"
#include <utils/WebXQuality.h>
#include <utils/WebXSettings.h>
#include <utils/WebXRectangle.h>
#include <models/WebXWindowDamage.h>

class WebXClientWindow {
public:
    WebXClientWindow(Window id, const WebXQuality & desiredQuality, const WebXRectangle & rectangle, const WebXWindowCoverage & coverage, const WebXQualitySettings & settings) :
        _id(id),
        _damage(id),
        _qualityHandler(id, desiredQuality, coverage, settings),
        _windowSize(rectangle.size()),
        _imageRefreshTime(std::chrono::high_resolution_clock::now()),
        _rgbChecksum(0),
        _alphaChecksum(0) {
    }
    WebXClientWindow(Window id, const WebXQuality & desiredQuality, const WebXWindowDamage & damage, const WebXQualitySettings & settings) :
        _id(id),
        _damage(damage),
        _qualityHandler(id, desiredQuality, settings),
        _imageRefreshTime(std::chrono::high_resolution_clock::now()),
        _rgbChecksum(0),
        _alphaChecksum(0) {
    }
    virtual ~WebXClientWindow() {}

    const Window & getId() const {
        return this->_id;
    }

    const WebXWindowDamage & getDamage() const {
        return this->_damage;
    }

    void addDamage(const WebXWindowDamage & damage) {
        this->_damage += damage;
    }

    void resetDamage() {
        this->_damage.reset();
    }

    bool hasDamage() const {
        return this->_damage.hasDamage();
    }

    bool isFullWindowDamage() const {
        return this->_damage.isFullWindow(this->_windowSize);
    }

    float getDamageAreaRatio() const {
        return (1.0) * this->_damage.getDamagedArea() / this->_windowSize.area(); 
    }

    const WebXQuality & getCurrentQuality() const {
        return this->_qualityHandler.getCurrentQuality();
    }

    void setSize(const WebXSize & size) {
        if (this->_windowSize != size) {
            this->_windowSize = size;
        }
    }

    void setCoverage(const WebXWindowCoverage & coverage) {
        this->_qualityHandler.setWindowCoverage(coverage);
    }

    bool requiresRefresh(const std::chrono::high_resolution_clock::time_point & reference) {
        return this->_imageRefreshTime < reference;
    }

    uint32_t getRGBChecksum() const {
        return this->_rgbChecksum;
    }

    void setRGBChecksum(uint32_t rgbChecksum) {
        this->_rgbChecksum = rgbChecksum;
    }

    uint32_t getAlphaChecksum() const {
        return this->_alphaChecksum;
    }

    void setAlphaChecksum(uint32_t alphaChecksum) {
        this->_alphaChecksum = alphaChecksum;
    }

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
    Window _id;
    WebXWindowDamage _damage;
    WebXWindowQualityHandler _qualityHandler;
    WebXSize _windowSize;

    std::chrono::high_resolution_clock::time_point _imageRefreshTime;

    uint32_t _rgbChecksum;
    uint32_t _alphaChecksum;
};


#endif /* WEBX_CLIENT_WINDOW_H */