/*
 * WebXTransferData.h
 * 
 * This header file defines the WebXTransferData class, which represents 
 * data transfer information in the WebX engine. It includes details such 
 * as the timestamp of the transfer and the size of the data in kilobytes.
 */

#ifndef WEBX_TRANSFER_DATA_H
#define WEBX_TRANSFER_DATA_H

#include <chrono>

/*
 * WebXTransferData
 * 
 * A class that encapsulates data transfer details. It stores the timestamp 
 * of when the data transfer occurred and the size of the data in kilobytes.
 */
class WebXTransferData {
public:
    /*
     * Constructor
     * 
     * Initializes a WebXTransferData object with the given size in kilobytes.
     * The timestamp is automatically set to the current time.
     * 
     * Parameters:
     * - sizeKB: The size of the data in kilobytes.
     */
    WebXTransferData(float sizeKB) :
        timestamp(std::chrono::high_resolution_clock::now()),
        sizeKB(sizeKB) {}

    /*
     * Destructor
     * 
     * A virtual destructor to allow proper cleanup in derived classes.
     */
    virtual ~WebXTransferData() {}

    /*
     * Member Variables
     * 
     * - timestamp: The time point when the data transfer occurred.
     * - sizeKB: The size of the data in kilobytes.
     */
    std::chrono::high_resolution_clock::time_point timestamp;
    float sizeKB;
};

#endif /* WEBX_TRANSFER_DATA_H */