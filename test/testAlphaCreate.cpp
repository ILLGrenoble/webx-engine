#include <stdlib.h>
#include <stdio.h>
#include <chrono>
#include <utils/WebXImageUtils.h>



inline void convertToAlpha1(u_int32_t * data, size_t length) {

    u_int32_t * pixel = (u_int32_t *)data;
    for (int i = 0; i < length; i++) {
        *pixel = (*pixel & 0xFF000000);
        pixel++;
    }

}

inline void convertToAlpha2(u_int32_t * data, size_t length) {

    u_int32_t * src = data;
    u_int32_t * dst = data;

    // enabling optimization (at least -O2) automatically unrolls the inner for-loop
    const size_t Unroll = 4;
    const size_t PixelsAtOnce = 4 * Unroll;
    const u_int32_t mask = 0xFF000000;

    size_t remaining = length;

    while (remaining >= PixelsAtOnce) {
        for (size_t unrolling = 0; unrolling < Unroll; unrolling++) {

            *dst++ = *src++ & mask;
            *dst++ = *src++ & mask;
            *dst++ = *src++ & mask;
            *dst++ = *src++ & mask;
        }

        remaining -= PixelsAtOnce;
    }

    // remaining 1 to 15 uint32
    while (remaining-- != 0) {
        *dst++ = *src++ & mask;
    }
}

int main() {

    srand(time(NULL));

    int width = 1680;
    int height = 1050;
    int length = width * height;

    size_t byteSize = length * 4;
    u_int8_t * data = (u_int8_t *)malloc(byteSize);

    // Initialise data
    u_int8_t * dataPtr = data;
    for (size_t i = 0; i < byteSize; i++) {
        // if (i % 4 == 3) {
        //     *dataPtr = 255;
        // } else {
            *dataPtr = (u_int8_t)rand();
        // }
        dataPtr++;
    }
    
    
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    int nIter = 1000;
    for (int it = 0; it < nIter; it++) {
        convertToAlpha1((u_int32_t *)data, length);
    }

    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> duration = end - start;
    double cummulativeTimeUs = duration.count();

    printf("Test completed: %d iterations in %fms, %fus / iteration for %luKB\n", nIter, cummulativeTimeUs, (cummulativeTimeUs / nIter), byteSize / 1024);
    
    start = std::chrono::high_resolution_clock::now();

    for (int it = 0; it < nIter; it++) {
        convertToAlpha2((u_int32_t *)data, length);
    }

    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    cummulativeTimeUs = duration.count();

    printf("Test completed: %d iterations in %fms, %fus / iteration for %luKB\n", nIter, cummulativeTimeUs, (cummulativeTimeUs / nIter), byteSize / 1024);
    
    start = std::chrono::high_resolution_clock::now();

    for (int it = 0; it < nIter; it++) {
        webx_convertToAlpha((u_int32_t *)data, length);
    }

    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    cummulativeTimeUs = duration.count();

    printf("Test completed: %d iterations in %fms, %fus / iteration for %luKB\n", nIter, cummulativeTimeUs, (cummulativeTimeUs / nIter), byteSize / 1024);

    free(data);
}

