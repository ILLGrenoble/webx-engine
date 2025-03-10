#ifndef WEBX_QUALITY_H
#define WEBX_QUALITY_H

#include <vector>
#include <spdlog/spdlog.h>

class WebXQuality {
public:
    WebXQuality(int index, float imageFPS, float rgbQuality, float alphaQuality, float maxMbps) :
        index(index),
        imageFPS(imageFPS),
        rgbQuality(rgbQuality),
        alphaQuality(alphaQuality),
        maxMbps(maxMbps),
        imageUpdateTimeUs(1000000.0 / imageFPS) {}

    WebXQuality(const WebXQuality & quality) :
        index(quality.index),
        imageFPS(quality.imageFPS),
        rgbQuality(quality.rgbQuality),
        alphaQuality(quality.alphaQuality),
        maxMbps(quality.maxMbps),
        imageUpdateTimeUs(quality.imageUpdateTimeUs) {}
        virtual ~WebXQuality() {}

    bool operator == (const WebXQuality & quality) const {
        return this->index == quality.index;
    }

    bool operator != (const WebXQuality & quality) const {
        return this->index != quality.index;
    }

    bool operator < (const WebXQuality & quality) const {
        return this->index < quality.index;
    }

    bool operator > (const WebXQuality & quality) const {
        return this->index > quality.index;
    }

    static const WebXQuality & MaxQuality() {
        return QUALITY_SETTINGS[QUALITY_SETTINGS.size() - 1];
    }


    static const WebXQuality & QualityForIndex(uint32_t qualityIndex) {
        if (qualityIndex < 1 || qualityIndex > 10) {
            spdlog::warn("Attempt to get the quality for an invalid index ({:d})", qualityIndex);
            qualityIndex = qualityIndex < 1 ? 1 : 10;
        }
            
        const WebXQuality & quality = QUALITY_SETTINGS[qualityIndex - 1];
            
        return quality;
    }
    
    static const WebXQuality & QualityForImageCoverageLinear(float coverage) {
        if (coverage < 0.0 || coverage > 1.0) {
            spdlog::warn("Attempt to get the quality for an invalid coverage ({:f})", coverage);
            coverage = coverage < 0.0 ? 0.0 : 1.0;
        }
    
        // Coverage [0.0:1.0], quality [1:10]
        int qualityIndex = std::ceil(10 - (9.99 * coverage));
    
        const WebXQuality & quality = QUALITY_SETTINGS[qualityIndex - 1];
    
        return quality;
    }
    
    static const WebXQuality & QualityForImageCoverageQuadratic(float coverage) {
        if (coverage < 0.0 || coverage > 1.0) {
            spdlog::warn("Attempt to get the quality for an invalid coverage ({:f})", coverage);
            coverage = coverage < 0.0 ? 0.0 : 1.0;
        }
    
        // Coverage [0.0:1.0], quality [1:10]
        // quadratic conversion from coverage to quality (keep higher quality for smaller coverage)
        int qualityIndex = std::ceil(10 - (9.99 * coverage * coverage));
    
        const WebXQuality & quality = QUALITY_SETTINGS[qualityIndex - 1];
    
        return quality;
    }
    
    static const WebXQuality & QualityForImageMbps(float imageMbps, const WebXQuality & desiredQuality, const WebXQuality & currentQuality) {
    
        float lowerBoundImageMbps = desiredQuality.index > 1 ? QualityForIndex(desiredQuality.index - 1).maxMbps : 0.8 * desiredQuality.maxMbps;
        if (imageMbps > desiredQuality.maxMbps && currentQuality.index > 1) {
            // Keep reducing the current quality until we get the desired KB/s rate
            return QualityForIndex(currentQuality.index - 1);
        
        } else if (imageMbps < lowerBoundImageMbps && currentQuality.index < MAX_QUALITY_INDEX) {
            // Increase the quality if the image KB/s is lower than the minimum for the desired quality 
            return QualityForIndex(currentQuality.index + 1);
        }
    
        return currentQuality;
    }

    int index;
    float imageFPS;
    float rgbQuality;
    float alphaQuality;
    float maxMbps;

    int imageUpdateTimeUs;

    static const std::vector<WebXQuality> QUALITY_SETTINGS;
    static const int MAX_QUALITY_INDEX = 10;

};

#endif /* WEBX_QUALITY_H */