#ifndef WEBX_IMAGE_UTILS_H
#define WEBX_IMAGE_UTILS_H

#include <stdlib.h>


inline int countTransparentPixels(const u_int32_t * data, size_t length) {

    int alphaCount = 0;

    const u_int32_t * current = data;

    // enabling optimization (at least -O2) automatically unrolls the inner for-loop
    const size_t Unroll = 4;
    const size_t PixelsAtOnce = 4 * Unroll;
    const u_int32_t mask = 0xFF000000;

    size_t remaining = length;

    while (remaining >= PixelsAtOnce) {
        for (size_t unrolling = 0; unrolling < Unroll; unrolling++) {

            u_int32_t one   = *current++;
            u_int32_t two   = *current++;
            u_int32_t three = *current++;
            u_int32_t four  = *current++;

            alphaCount += (one & mask) != mask;
            alphaCount += (two & mask) != mask;
            alphaCount += (three & mask) != mask;
            alphaCount += (four & mask) != mask;
        }

        remaining -= PixelsAtOnce;
    }

    // remaining 1 to 15 uint32
    while (remaining-- != 0) {
        u_int32_t value  = *current++;
        alphaCount += (value & mask) != mask;
    }

    return alphaCount;
}


inline int comparePixels(const u_int32_t * data1, const u_int32_t * data2, size_t length) {

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



#endif /* WEBX_IMAGE_UTILS_H */