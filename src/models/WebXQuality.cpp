#include "WebXQuality.h"

const std::vector<WebXQuality> WebXQuality::QUALITY_SETTINGS = {
    WebXQuality(1, 0.5, 0.4, 0.5, 0.5), // 64 KB/s
    WebXQuality(2, 0.5, 0.4, 0.5, 0.75), // 96 KB/S
    WebXQuality(3, 1, 0.5, 0.6, 1.0), // 128 KB/s
    WebXQuality(4, 3, 0.6, 0.6, 2.0), // 256 KB/s
    WebXQuality(5, 5, 0.6, 0.7, 3.0), // 384 KB/s
    WebXQuality(6, 6, 0.7, 0.7, 4.0), // 512 KB/s
    WebXQuality(7, 8, 0.7, 0.8, 5.0), // 640 KB/s
    WebXQuality(8, 10, 0.8, 0.8, 6.0), // 768 KB/s
    WebXQuality(9, 12, 0.9, 0.9, 8.0), // 1024 KB/s
    WebXQuality(10, 15, 0.9, 0.9, 12.0), // 1536 KB/s
};