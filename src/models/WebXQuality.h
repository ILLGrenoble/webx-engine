#ifndef WEBX_QUALITY_H
#define WEBX_QUALITY_H

#include <vector>
#include <spdlog/spdlog.h>

/**
 * @class WebXQuality
 * @brief Represents quality settings for image transfer, including FPS, quality levels, and bandwidth limits.
 */
class WebXQuality {
public:
    /**
     * @brief Constructs a WebXQuality object with specified parameters.
     * @param index The quality index (1-12).
     * @param imageFPS The frames per second for image updates.
     * @param rgbQuality The quality level for RGB channels.
     * @param alphaQuality The quality level for alpha channels.
     * @param maxMbps The maximum bandwidth in Mbps.
     */
    WebXQuality(int index, float imageFPS, float rgbQuality, float alphaQuality, float maxMbps) :
        index(index),
        imageFPS(imageFPS),
        rgbQuality(rgbQuality),
        alphaQuality(alphaQuality),
        maxMbps(maxMbps),
        imageUpdateTimeUs(1000000.0 / imageFPS) {}

    /**
     * @brief Copy constructor for WebXQuality.
     * @param quality The WebXQuality object to copy from.
     */
    WebXQuality(const WebXQuality & quality) :
        index(quality.index),
        imageFPS(quality.imageFPS),
        rgbQuality(quality.rgbQuality),
        alphaQuality(quality.alphaQuality),
        maxMbps(quality.maxMbps),
        imageUpdateTimeUs(quality.imageUpdateTimeUs) {}
        virtual ~WebXQuality() {}

    /**
     * @brief Equality operator for WebXQuality.
     * @param quality The WebXQuality object to compare with.
     * @return True if both qualities have the same index, false otherwise.
     */
    bool operator == (const WebXQuality & quality) const {
        return this->index == quality.index;
    }

    /**
     * @brief Inequality operator for WebXQuality.
     * @param quality The WebXQuality object to compare with.
     * @return True if the qualities have different indices, false otherwise.
     */
    bool operator != (const WebXQuality & quality) const {
        return this->index != quality.index;
    }

    /**
     * @brief Less-than operator for WebXQuality.
     * @param quality The WebXQuality object to compare with.
     * @return True if this quality's index is less than the other's, false otherwise.
     */
    bool operator < (const WebXQuality & quality) const {
        return this->index < quality.index;
    }

    /**
     * @brief Greater-than operator for WebXQuality.
     * @param quality The WebXQuality object to compare with.
     * @return True if this quality's index is greater than the other's, false otherwise.
     */
    bool operator > (const WebXQuality & quality) const {
        return this->index > quality.index;
    }

    /**
     * @brief Gets the maximum quality setting.
     * @return A reference to the maximum WebXQuality object.
     */
    static const WebXQuality & MaxQuality() {
        return QUALITY_SETTINGS[MaxRuntimeQualityIndex - 1];
    }

    /**
     * @brief Gets the quality setting for a specific index.
     * @param qualityIndex The quality index (1-MaxRuntimeQualityIndex).
     * @return A reference to the WebXQuality object for the specified index.
     */
    static const WebXQuality & QualityForIndex(uint32_t qualityIndex) {
        if (qualityIndex < 1 || qualityIndex > MaxRuntimeQualityIndex) {
            spdlog::warn("Attempt to get the quality for an invalid index ({:d})", qualityIndex);
            qualityIndex = qualityIndex < 1 ? 1 : MaxRuntimeQualityIndex;
        }
            
        const WebXQuality & quality = QUALITY_SETTINGS[qualityIndex - 1];
            
        return quality;
    }
    
    static const WebXQuality & QualityForImageCoverageLinear(float coverage) {
        if (coverage < 0.0 || coverage > 1.0) {
            spdlog::warn("Attempt to get the quality for an invalid coverage ({:f})", coverage);
            coverage = coverage < 0.0 ? 0.0 : 1.0;
        }
    
        // Coverage [0.0:1.0], quality [1:MaxRuntimeQualityIndex]
        int qualityIndex = std::ceil(MaxRuntimeQualityIndex - ((MaxRuntimeQualityIndex - 0.01) * coverage));
    
        const WebXQuality & quality = QUALITY_SETTINGS[qualityIndex - 1];
    
        return quality;
    }
    
    static const WebXQuality & QualityForImageCoverageQuadratic(float coverage) {
        if (coverage < 0.0 || coverage > 1.0) {
            spdlog::warn("Attempt to get the quality for an invalid coverage ({:f})", coverage);
            coverage = coverage < 0.0 ? 0.0 : 1.0;
        }
    
        // Coverage [0.0:1.0], quality [1:MaxRuntimeQualityIndex]
        // quadratic conversion from coverage to quality (keep higher quality for smaller coverage)
        int qualityIndex = std::ceil(MaxRuntimeQualityIndex - ((MaxRuntimeQualityIndex - 0.01) * coverage * coverage));
    
        const WebXQuality & quality = QUALITY_SETTINGS[qualityIndex - 1];
    
        return quality;
    }
    
    static const WebXQuality & QualityForImageMbps(float imageMbps, const WebXQuality & desiredQuality, const WebXQuality & currentQuality) {
    
        float lowerBoundImageMbps = desiredQuality.index > 1 ? QualityForIndex(desiredQuality.index - 1).maxMbps : 0.8 * desiredQuality.maxMbps;
        if (imageMbps > desiredQuality.maxMbps && currentQuality.index > 1) {
            // Keep reducing the current quality until we get the desired KB/s rate
            return QualityForIndex(currentQuality.index - 1);
        
        } else if (imageMbps < lowerBoundImageMbps && currentQuality.index < MaxRuntimeQualityIndex) {
            // Increase the quality if the image KB/s is lower than the minimum for the desired quality 
            return QualityForIndex(currentQuality.index + 1);
        }
    
        return currentQuality;
    }

    static void SetRuntimeMaxQualityIndex(int maxRuntimeQualityIndex) {
        if (maxRuntimeQualityIndex < 1) {
            maxRuntimeQualityIndex = 1;

        } else if (maxRuntimeQualityIndex > MAX_QUALITY_INDEX) {
            maxRuntimeQualityIndex = MAX_QUALITY_INDEX;
        }
        
        MaxRuntimeQualityIndex = maxRuntimeQualityIndex;
    }

    int index;
    float imageFPS;
    float rgbQuality;
    float alphaQuality;
    float maxMbps;

    int imageUpdateTimeUs;

private:
    static const std::vector<WebXQuality> QUALITY_SETTINGS;
    static const int MAX_QUALITY_INDEX = 12;
    static int MaxRuntimeQualityIndex;

};

#endif /* WEBX_QUALITY_H */