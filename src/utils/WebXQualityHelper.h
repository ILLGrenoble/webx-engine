#ifndef WEBX_QUALITY_HELPER_H
#define WEBX_QUALITY_HELPER_H

#include <spdlog/spdlog.h>
#include <algorithm>
#include "WebXQuality.h"

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

    int qualityIndex = std::ceil((1.0 - coverage) * 10);

    const WebXQuality & quality = WebXQuality::QUALITY_SETTINGS[qualityIndex - 1];

    return quality;
}

#endif /* WEBX_QUALITY_HELPER_H */