#ifndef WEBX_QUALITY_H
#define WEBX_QUALITY_H

#include <vector>

class WebXQuality {
public:
    WebXQuality(int index, float imageFPS, float imageQuality) :
        index(index),
        imageFPS(imageFPS),
        imageQuality(imageQuality),
        imageUpdateTimeUs(1000000.0 / imageFPS) {}
    virtual ~WebXQuality() {}

    int index;
    float imageFPS;
    float imageQuality;
    double imageUpdateTimeUs;

    static const std::vector<WebXQuality> QUALITY_SETTINGS;
    static const int MAX_QUALITY_INDEX = 10;

};

#endif /* WEBX_QUALITY_H */