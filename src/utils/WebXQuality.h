#ifndef WEBX_QUALITY_H
#define WEBX_QUALITY_H

#include <vector>

class WebXQuality {
public:
    WebXQuality(int index, int imageFPS, float imageQuality) :
        index(index),
        imageFPS(imageFPS),
        imageQuality(imageQuality),
        imageUpdateTimeUs(1000000.0 / imageFPS) {}
    virtual ~WebXQuality() {}

    int index;
    int imageFPS;
    float imageQuality;
    double imageUpdateTimeUs;

    static const std::vector<WebXQuality> QUALITY_SETTINGS;

};

#endif /* WEBX_QUALITY_H */