#ifndef WEBX_HEX_UTILS_H
#define WEBX_HEX_UTILS_H

#include "WebXResult.h"
#include <cstdint>
#include <string>
#include <spdlog/spdlog.h>

/**
 * Utility class for converting hexadecimal strings to numeric types.
 */
class HexUtils {
public:
    /**
     * Converts a hexadecimal string to a 32-bit unsigned integer.
     * 
     * @param hex The hexadecimal string to convert.
     * @return A WebXResult containing the converted value or an error message.
     */
    static WebXResult<uint32_t> toUint32(const std::string & hex) {
        try {
            return WebXResult<uint32_t>::Ok(std::stoul(hex, nullptr, 16));
        
        } catch (const std::exception& e) {
            return WebXResult<uint32_t>::Err(fmt::format("Invalid hex string {:x}", hex));
        }
    }

    /**
     * Converts a hexadecimal string to a 64-bit unsigned integer.
     * 
     * @param hex The hexadecimal string to convert.
     * @return A WebXResult containing the converted value or an error message.
     */
    static WebXResult<uint64_t> toUint64(const std::string & hex) {
        try {
            return WebXResult<uint64_t>::Ok(std::stoul(hex, nullptr, 16));
        
        } catch (const std::exception& e) {
            return WebXResult<uint64_t>::Err(fmt::format("Invalid hex string {:x}", hex));
        }
    }
};

#endif /* WEBX_HEX_UTILS_H */