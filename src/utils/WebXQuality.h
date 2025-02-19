#ifndef WEBX_QUALITY_H
#define WEBX_QUALITY_H

#include <vector>

class WebXQuality {
public:
    WebXQuality(int index, float imageFPS, float rgbQuality, float alphaQuality, float maxKbps) :
        index(index),
        imageFPS(imageFPS),
        rgbQuality(rgbQuality),
        alphaQuality(alphaQuality),
        maxKbps(maxKbps),
        imageUpdateTimeUs(1000000.0 / imageFPS) {}
    virtual ~WebXQuality() {}

    int index;
    float imageFPS;
    float rgbQuality;
    float alphaQuality;
    float maxKbps;

    double imageUpdateTimeUs;

    static const std::vector<WebXQuality> QUALITY_SETTINGS;
    static const int MAX_QUALITY_INDEX = 10;

};

#endif /* WEBX_QUALITY_H */