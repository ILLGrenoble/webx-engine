#ifndef WEBX_STRING_UTILS_H
#define WEBX_STRING_UTILS_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

/**
 * Utility class for string operations.
 */
class StringUtils {
public:
    /**
     * Splits a string into a vector of substrings based on a delimiter.
     * 
     * @param str The input string to split.
     * @param delimiter The character used as the delimiter.
     * @return A vector of substrings.
     */
    static std::vector<std::string> split(const std::string & str, char delimiter) {
        std::vector<std::string> tokens;
        std::stringstream stringstream(str);
        std::string token;

        while (std::getline(stringstream, token, delimiter)) {
            tokens.push_back(token);
        }

        return tokens;
    }
};

#endif /* WEBX_STRING_UTILS_H */