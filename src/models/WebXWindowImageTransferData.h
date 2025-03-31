#ifndef WEBX_WINDOW_IMAGE_TRANSFER_H
#define WEBX_WINDOW_IMAGE_TRANSFER_H

#include <X11/Xlib.h>
#include <memory>
#include <chrono>

/**
 * @class WebXWindowImageTransferData
 * @brief Represents data related to the image transfer of an X11 window.
 */
class WebXWindowImageTransferData {
public:
    /**
     * @enum WebXWindowImageTransferStatus
     * @brief Enum representing the status of the image transfer.
     */
    enum WebXWindowImageTransferStatus {
        FullWindow = 0, /**< Full window image transfer. */
        SubWindow,      /**< Partial window image transfer. */
        Ignored         /**< Ignored image transfer. */
    };

    /**
     * @brief Constructs a WebXWindowImageTransferData object with a status.
     * @param x11Window The X11 window handle.
     * @param status The image transfer status.
     */
    WebXWindowImageTransferData(Window x11Window, WebXWindowImageTransferStatus status) :
        x11Window(x11Window),
        timestamp(std::chrono::high_resolution_clock::now()),
        imageSizeKB(0),
        rgbChecksum(0),
        alphaChecksum(0),
        status(status) {}

    /**
     * @brief Constructs a WebXWindowImageTransferData object with image size.
     * @param x11Window The X11 window handle.
     * @param imageSizeKB The size of the image in kilobytes.
     */
    WebXWindowImageTransferData(Window x11Window, float imageSizeKB) :
        x11Window(x11Window),
        timestamp(std::chrono::high_resolution_clock::now()),
        imageSizeKB(imageSizeKB),
        rgbChecksum(0),
        alphaChecksum(0),
        status(SubWindow) {}

    /**
     * @brief Constructs a WebXWindowImageTransferData object with image size and checksums.
     * @param x11Window The X11 window handle.
     * @param imageSizeKB The size of the image in kilobytes.
     * @param rgbChecksum The RGB checksum of the image.
     * @param alphaChecksum The alpha checksum of the image.
     */
    WebXWindowImageTransferData(Window x11Window, float imageSizeKB, uint32_t rgbChecksum, uint32_t alphaChecksum) :
        x11Window(x11Window),
        timestamp(std::chrono::high_resolution_clock::now()),
        imageSizeKB(imageSizeKB),
        rgbChecksum(rgbChecksum),
        alphaChecksum(alphaChecksum),
        status(FullWindow) {}

    /**
     * @brief Copy constructor for WebXWindowImageTransferData.
     * @param transferData The object to copy from.
     */
    WebXWindowImageTransferData(const WebXWindowImageTransferData & transferData) :
        x11Window(transferData.x11Window),
        timestamp(transferData.timestamp),
        imageSizeKB(transferData.imageSizeKB),
        rgbChecksum(transferData.rgbChecksum),
        alphaChecksum(transferData.alphaChecksum),
        status(transferData.status) {}

    /**
     * @brief Destructor for WebXWindowImageTransferData.
     */
    virtual ~WebXWindowImageTransferData() {
    }

    const Window x11Window;
    const std::chrono::high_resolution_clock::time_point timestamp;
    const float imageSizeKB;
    const uint32_t rgbChecksum;
    const uint32_t alphaChecksum;
    const WebXWindowImageTransferStatus status;
};

#endif /* WEBX_WINDOW_IMAGE_TRANSFER_H */