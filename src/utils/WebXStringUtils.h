#ifndef WEBX_STRING_UTILS_H
#define WEBX_STRING_UTILS_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

/**
 * Utility class for string operations.
 */
class WebXStringUtils {
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


    /**
     * Compares the end of a given string to determine if it matches the given ending
     * 
     * @param fullString the string to test
     * @param ending the ending to find
     * @return true if the string ends with the given ending
     */
    static bool hasEnding(std::string const &fullString, std::string const &ending) {
        if (fullString.length() >= ending.length()) {
            return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
        } else {
            return false;
        }
    }


};

#endif /* WEBX_STRING_UTILS_H */