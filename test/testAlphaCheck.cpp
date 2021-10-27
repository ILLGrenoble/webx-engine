#include <stdlib.h>
#include <stdio.h>
#include <chrono>
#include <emmintrin.h>
#include <tmmintrin.h>
#include <utils/WebXImageUtils.h>


int calcTransparentPixels(u_int32_t * data, size_t length) {
    int hasAlpha = 0;
    u_int32_t * pixel = data;
    for (int i = 0; i < length; i++) {
        uint32_t pixelValue = *pixel & 0xFF000000;
        if (pixelValue != 0xFF000000) {
            hasAlpha++;
        }
        pixel++;
    }

    return hasAlpha;
}

int calcTransparentPixels2(u_int32_t * data, size_t length) {
    int hasAlpha = 0;

    const uint32_t * current = data;

    // enabling optimization (at least -O2) automatically unrolls the inner for-loop
    const size_t Unroll = 4;
    const size_t PixelsAtOnce = 4 * Unroll;
    const uint32_t mask = 0xFF000000;

    while (length >= PixelsAtOnce) {
        for (size_t unrolling = 0; unrolling < Unroll; unrolling++) {

            uint32_t one   = *current++;
            uint32_t two   = *current++;
            uint32_t three = *current++;
            uint32_t four  = *current++;

            hasAlpha += (one & mask) != mask;
            hasAlpha += (two & mask) != mask;
            hasAlpha += (three & mask) != mask;
            hasAlpha += (four & mask) != mask;
        }

        length -= PixelsAtOnce;
    }

    // remaining 1 to 3 uint32
    while (length-- != 0) {
        uint32_t value  = *current++;
        hasAlpha += (value & mask) != mask;
    }

    return hasAlpha;
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
        if (i % 4 == 3) {
            *dataPtr = 255;
        } else {
            *dataPtr = (u_int8_t)rand();
        }
        dataPtr++;
    }
    
    
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    int nIter = 1000;
    int hasAlpha = 0;
    for (int it = 0; it < nIter; it++) {
        hasAlpha += calcTransparentPixels((u_int32_t *)data, length);
    }

    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> duration = end - start;
    double cummulativeTimeUs = duration.count();

    printf("Transparent count = %d in %d iterations\n", hasAlpha / nIter, nIter);
    printf("Base test completed: %d iterations in %fms, %fus / iteration for %luKB\n", nIter, cummulativeTimeUs, (cummulativeTimeUs / nIter), byteSize / 1024);
    
    start = std::chrono::high_resolution_clock::now();
    hasAlpha = 0;
    for (int it = 0; it < nIter; it++) {
        hasAlpha += calcTransparentPixels2((u_int32_t *)data, length);
    }

    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    cummulativeTimeUs = duration.count();

    printf("Transparent count = %d in %d iterations\n", hasAlpha / nIter, nIter);
    printf("Fast test completed: %d iterations in %fms, %fus / iteration for %luKB\n", nIter, cummulativeTimeUs, (cummulativeTimeUs / nIter), byteSize / 1024);

    start = std::chrono::high_resolution_clock::now();
    hasAlpha = 0;
    for (int it = 0; it < nIter; it++) {
        hasAlpha += webx_countTransparentPixels((const u_int32_t *)data, length);
    }

    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    cummulativeTimeUs = duration.count();

    printf("Transparent count = %d in %d iterations\n", hasAlpha / nIter, nIter);
    printf("ImageUtils test completed: %d iterations in %fms, %fus / iteration for %luKB\n", nIter, cummulativeTimeUs, (cummulativeTimeUs / nIter), byteSize / 1024);

    free(data);
}

