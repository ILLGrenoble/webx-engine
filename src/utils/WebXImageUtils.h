#ifndef WEBX_IMAGE_UTILS_H
#define WEBX_IMAGE_UTILS_H

#include <stdlib.h>

/**
 * Counts the number of transparent pixels in the given image data.
 * 
 * @param data Pointer to the image data.
 * @param length The number of pixels in the image.
 * @param exitOnFound If true, stops counting after finding the first transparent pixel.
 * @return The number of transparent pixels.
 */
inline int webx_countTransparentPixels(const u_int32_t * data, size_t length, bool exitOnFound = false) {

    int alphaCount = 0;

    const u_int32_t * current = data;

    // enabling optimization (at least -O2) automatically unrolls the inner for-loop
    const size_t Unroll = 4;
    const size_t PixelsAtOnce = 4 * Unroll;
    const u_int32_t mask = 0xFF000000;

    size_t remaining = length;
    bool exit = false;

    while (remaining >= PixelsAtOnce && !exit) {
        for (size_t unrolling = 0; unrolling < Unroll; unrolling++) {

            u_int32_t one   = *current++;
            u_int32_t two   = *current++;
            u_int32_t three = *current++;
            u_int32_t four  = *current++;

            alphaCount += (one & mask) != mask;
            alphaCount += (two & mask) != mask;
            alphaCount += (three & mask) != mask;
            alphaCount += (four & mask) != mask;

            exit = alphaCount > 0 && exitOnFound;
        }

        remaining -= PixelsAtOnce;
    }

    // remaining 1 to 15 uint32
    while (remaining-- != 0 && !exit) {
        u_int32_t value  = *current++;
        alphaCount += (value & mask) != mask;
        exit = alphaCount > 0 && exitOnFound;
    }

    return alphaCount;
}

/**
 * Converts the given image data to alpha-only format.
 * 
 * @param data Pointer to the image data.
 * @param length The number of pixels in the image.
 */
inline void webx_convertToAlpha(u_int32_t * data, size_t length) {

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

    // slow (check testAlphaCreate)
    // Simple pixel iteration test completed: 1000 iterations in 297582.459000ms, 297.582459us / iteration for 6890KB
    // Loop unrolling test completed: 1000 iterations in 38340.459000ms, 38.340459us / iteration for 6890KB
    // u_int32_t * pixel = (u_int32_t *)data;
    // for (int i = 0; i < length; i++) {
    //     *pixel = (*pixel & 0xFF000000);
    //     pixel++;
    // }

}

#endif /* WEBX_IMAGE_UTILS_H */