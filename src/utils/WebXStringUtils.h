#ifndef WEBX_STRING_UTILS_H
#define WEBX_STRING_UTILS_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

class StringUtils {
public:
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