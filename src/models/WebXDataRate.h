#ifndef WEBX_DATA_RATE_H
#define WEBX_DATA_RATE_H

class WebXDataRate {
public:
    WebXDataRate() : valid(false), Mbps(0.0) {}
    WebXDataRate(float bitrateMbps) : valid(true), Mbps(bitrateMbps) {}
    virtual ~WebXDataRate() {}

    bool valid;
    float Mbps;
};


#endif /* WEBX_DATA_RATE_H */