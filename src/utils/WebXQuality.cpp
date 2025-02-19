#include "WebXQuality.h"

const std::vector<WebXQuality> WebXQuality::QUALITY_SETTINGS = {
    WebXQuality(1, 0.5, 0.4, 0.5, 64),
    WebXQuality(2, 0.5, 0.4, 0.5, 96),
    WebXQuality(3, 1, 0.5, 0.6, 128),
    WebXQuality(4, 3, 0.6, 0.6, 256),
    WebXQuality(5, 5, 0.6, 0.7, 384),
    WebXQuality(6, 6, 0.7, 0.7, 512),
    WebXQuality(7, 8, 0.7, 0.8, 640),
    WebXQuality(8, 10, 0.8, 0.8, 768),
    WebXQuality(9, 12, 0.9, 0.9, 1024),
    WebXQuality(10, 15, 0.9, 0.9, 1536),
};