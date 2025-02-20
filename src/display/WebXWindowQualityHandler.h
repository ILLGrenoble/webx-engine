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

private:
    const static int DATA_RETENTION_TIME_MS = 4000;
    const static int NO_RECENT_WINDOW_UPDATE_TIME_MS = 2000;
    const static int TIME_FOR_VALID_IMAGE_KBPS_MS = 1000;
    
    unsigned long _windowId;

    WebXRectangle::WebXRectCoverage _coverage;
    WebXQuality _coverageQuality;
    WebXQuality _imageKbpsQuality;
    WebXQuality _currentQuality;
    
    std::vector<WebXWindowQualityData> _dataStore;
    WebXWindowImageKbs _imageKbps;
    std::chrono::high_resolution_clock::time_point _imageKbpsInitTime;
};


#endif /* WEBX_WINDOW_QUALITY_HANDLER_H */