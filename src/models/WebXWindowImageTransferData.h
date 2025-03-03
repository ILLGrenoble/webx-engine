#ifndef WEBX_WINDOW_IMAGE_TRANSFER_H
#define WEBX_WINDOW_IMAGE_TRANSFER_H

#include <X11/Xlib.h>
#include <memory>
#include <chrono>

class WebXWindowImageTransferData {
public:
enum WebXWindowImageTransferStatus {
    FullWindow = 0,
    SubWindow,
    Ignored,
};

public:
    WebXWindowImageTransferData(Window x11Window, WebXWindowImageTransferStatus status) :
        x11Window(x11Window),
        timestamp(std::chrono::high_resolution_clock::now()),
        imageSizeKB(0),
        rgbChecksum(0),
        alphaChecksum(0),
        status(status) {}
    WebXWindowImageTransferData(Window x11Window, float imageSizeKB) :
        x11Window(x11Window),
        timestamp(std::chrono::high_resolution_clock::now()),
        imageSizeKB(imageSizeKB),
        rgbChecksum(0),
        alphaChecksum(0),
        status(SubWindow) {}
    WebXWindowImageTransferData(Window x11Window, float imageSizeKB, uint32_t rgbChecksum, uint32_t alphaChecksum) :
        x11Window(x11Window),
        timestamp(std::chrono::high_resolution_clock::now()),
        imageSizeKB(imageSizeKB),
        rgbChecksum(rgbChecksum),
        alphaChecksum(alphaChecksum),
        status(FullWindow) {}
    WebXWindowImageTransferData(const WebXWindowImageTransferData & transferData) :
        x11Window(transferData.x11Window),
        timestamp(transferData.timestamp),
        imageSizeKB(transferData.imageSizeKB),
        rgbChecksum(transferData.rgbChecksum),
        alphaChecksum(transferData.alphaChecksum),
        status(transferData.status) {}
    virtual ~WebXWindowImageTransferData() {
    }

    const Window x11Window;
    std::chrono::high_resolution_clock::time_point timestamp;
    const float imageSizeKB;
    const uint32_t rgbChecksum;
    const uint32_t alphaChecksum;
    const WebXWindowImageTransferStatus status;
};


#endif /* WEBX_WINDOW_IMAGE_TRANSFER_H */