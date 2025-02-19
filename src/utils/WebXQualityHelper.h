#ifndef WEBX_QUALITY_HELPER_H
#define WEBX_QUALITY_HELPER_H

#include <spdlog/spdlog.h>
#include <algorithm>
#include "WebXQuality.h"

inline const WebXQuality & webx_quality_max() {
    return WebXQuality::QUALITY_SETTINGS[WebXQuality::QUALITY_SETTINGS.size() - 1];
}

inline const WebXQuality & webx_quality_for_index(uint32_t qualityIndex) {
    if (qualityIndex < 1 || qualityIndex > 10) {
        spdlog::warn("Attempt to get the quality for an invalid index ({:d})", qualityIndex);
        qualityIndex = qualityIndex < 1 ? 1 : 10;
    }
        
    const WebXQuality & quality = WebXQuality::QUALITY_SETTINGS[qualityIndex - 1];
        
    return quality;
}

inline const WebXQuality & webx_quality_for_image_coverage(float coverage) {
    if (coverage < 0.0 || coverage > 1.0) {
        spdlog::warn("Attempt to get the quality for an invalid coverage ({:f})", coverage);
        coverage = coverage < 0.0 ? 0.0 : 1.0;
    }

    // Coverage [0.0:1.0], quality [1:10]
    int qualityIndex = 10 - (int)(9 * coverage);

    const WebXQuality & quality = WebXQuality::QUALITY_SETTINGS[qualityIndex - 1];

    return quality;
}

inline const WebXQuality & webx_quality_for_image_kbps(float imageKbps, const WebXQuality & desiredQuality, const WebXQuality & currentQuality) {

    float lowerBoundImageKbps = desiredQuality.index > 1 ? webx_quality_for_index(desiredQuality.index - 1).maxKbps : 0;
    if (imageKbps > desiredQuality.maxKbps && currentQuality.index > 1) {
        // Keep reducing the current quality until we get the desired KB/s rate
        return webx_quality_for_index(currentQuality.index - 1);
    
    } else if (imageKbps < lowerBoundImageKbps && currentQuality.index < desiredQuality.index) {
        // Increase the quality if the image KB/s is lower than the minimum for the desired quality 
        return webx_quality_for_index(currentQuality.index + 1);
    }

    return currentQuality;
}

#endif /* WEBX_QUALITY_HELPER_H */