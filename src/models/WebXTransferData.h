#ifndef WEBX_TRANSFER_DATA_H
#define WEBX_TRANSFER_DATA_H

#include <chrono>

class WebXTransferData {
public:
    WebXTransferData(float sizeKB) :
        timestamp(std::chrono::high_resolution_clock::now()),
        sizeKB(sizeKB) {}
    virtual ~WebXTransferData() {}

    std::chrono::high_resolution_clock::time_point timestamp;
    float sizeKB;
};


#endif /* WEBX_TRANSFER_DATA_H */