#include <stdlib.h>
#include <stdio.h>
#include <cstring>
#include <chrono>


int comparePixels1(const u_int32_t * data1, const u_int32_t * data2, size_t length) {
    int diffCount = 0;
    const u_int32_t * pixel1 = data1;
    const u_int32_t * pixel2 = data2;
    for (int i = 0; i < length; i++) {
        uint32_t pixel1Value = *pixel1;
        uint32_t pixel2Value = *pixel2;
        diffCount += pixel1Value != pixel2Value;
        pixel1++;
        pixel2++;
    }

    return diffCount;
}

int comparePixels2(const u_int32_t * data1, const u_int32_t * data2, size_t length) {

    int diffCount = 0;

    const u_int32_t * current1 = data1;
    const u_int32_t * current2 = data2;

    // enabling optimization (at least -O2) automatically unrolls the inner for-loop
    const size_t Unroll = 4;
    const size_t PixelsAtOnce = 4 * Unroll;

    size_t remaining = length;

    while (remaining >= PixelsAtOnce) {
        for (size_t unrolling = 0; unrolling < Unroll; unrolling++) {

            u_int32_t d1one   = *current1++;
            u_int32_t d1two   = *current1++;
            u_int32_t d1three = *current1++;
            u_int32_t d1four  = *current1++;

            u_int32_t d2one   = *current2++;
            u_int32_t d2two   = *current2++;
            u_int32_t d2three = *current2++;
            u_int32_t d2four  = *current2++;

            diffCount += d1one != d2one;
            diffCount += d1two != d2two;
            diffCount += d1three != d2three;
            diffCount += d1four != d2four;
        }

        remaining -= PixelsAtOnce;
    }

    // remaining 1 to 15 uint32
    while (remaining-- != 0) {
        u_int32_t d1value = *current1++;
        u_int32_t d2value = *current2++;
        diffCount += d1value != d2value;
    }

    return diffCount;
}

int comparePixels3(const u_int32_t * data1, const u_int32_t * data2, size_t length) {

    int block = 1680;
    int blockBytes = block * 4;
    int remaining = length;
    int diffCount = 0;
    const u_int32_t * ptr1 = data1;
    const u_int32_t * ptr2 = data2;

    while (remaining >= block) {
        diffCount += (memcmp(ptr1, ptr2, blockBytes) != 0);
        ptr1 += block;
        ptr2 += block;
        remaining -= block;
    }

    if (remaining > 0) {
        diffCount += (memcmp(ptr1, ptr2, remaining * 4) != 0);
    }

    return diffCount;
}


int main() {

    printf("NOTE! This test must be run when compiled for Release!\n");

    srand(time(NULL));

    int width = 1680;
    int height = 1050;
    int length = width * height;

    size_t byteSize = length * 4;
    u_int8_t * data1 = (u_int8_t *)malloc(byteSize);
    u_int8_t * data2 = (u_int8_t *)malloc(byteSize);

    // Initialise data1
    u_int8_t * dataPtr = data1;
    for (size_t i = 0; i < byteSize; i++) {
        *dataPtr++ = (u_int8_t)rand();
    }
    
    // Initialise data2
    memcpy(data2, data1, byteSize);

    // modify data2
    int deltaX = 250;
    int deltaY = 250;
    int deltaW = 100;
    int deltaH = 100;
    for (int j = 0; j < deltaH; j++) {
        u_int32_t * ptr = (u_int32_t *)data2 + (deltaY + j) * width + deltaX;
        for (int i = 0; i < deltaW; i++) {
            *ptr++ = 0xFFFFFFFF;
        }
    }
    
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    int nIter = 1000;
    int diffCount = 0;
    for (int it = 0; it < nIter; it++) {
        diffCount += comparePixels1((const u_int32_t *)data1, (const u_int32_t *)data2, length);
    }

    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::micro> duration = end - start;
    double cummulativeTimeUs = duration.count();

    printf("Difference count = %d in %d iterations\n", diffCount / nIter, nIter);
    printf("Base test completed: %d iterations in %fms, %fus / iteration for %luKB\n", nIter, cummulativeTimeUs, (cummulativeTimeUs / nIter), byteSize / 1024);

    start = std::chrono::high_resolution_clock::now();

    diffCount = 0;
    for (int it = 0; it < nIter; it++) {
        diffCount += comparePixels2((const u_int32_t *)data1, (const u_int32_t *)data2, length);
    }

    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    cummulativeTimeUs = duration.count();

    printf("Difference count = %d in %d iterations\n", diffCount / nIter, nIter);
    printf("Fast test completed: %d iterations in %fms, %fus / iteration for %luKB\n", nIter, cummulativeTimeUs, (cummulativeTimeUs / nIter), byteSize / 1024);

    start = std::chrono::high_resolution_clock::now();

    diffCount = 0;
    for (int it = 0; it < nIter; it++) {
        diffCount += comparePixels3((const u_int32_t *)data1, (const u_int32_t *)data2, length);
    }

    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    cummulativeTimeUs = duration.count();

    printf("Difference count = %d in %d iterations\n", diffCount / nIter, nIter);
    printf("Memcmp test completed: %d iterations in %fms, %fus / iteration for %luKB\n", nIter, cummulativeTimeUs, (cummulativeTimeUs / nIter), byteSize / 1024);


    start = std::chrono::high_resolution_clock::now();

    diffCount = 0;
    for (int it = 0; it < nIter; it++) {
        for (int j = 0; j < height; j++) {
            const u_int32_t * line1 = (const u_int32_t *)data1 + j * width;
            const u_int32_t * line2 = (const u_int32_t *)data2 + j * width;
            diffCount += comparePixels2(line1, line2, width);
        }
    }

    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    cummulativeTimeUs = duration.count();

    printf("Difference count = %d in %d iterations\n", diffCount / nIter, nIter);
    printf("Fast test completed: %d iterations in %fms, %fus / iteration for %luKB\n", nIter, cummulativeTimeUs, (cummulativeTimeUs / nIter), byteSize / 1024);


    free(data1);
    free(data2);
}

