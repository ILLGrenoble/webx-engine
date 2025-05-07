#ifndef WEBX_VERSION_H
#define WEBX_VERSION_H

#include <string>
#include <sstream>
#include <vector>
#include <spdlog/spdlog.h>
#include <cstdint>

class WebXVersion {

public:
    WebXVersion(const std::string& versionStr) :
        major(parseAll(versionStr).major),
        minor(parseAll(versionStr).minor),
        patch(parseAll(versionStr).patch) {}
    WebXVersion() :
        major(0),
        minor(0),
        patch(0) {}
    WebXVersion(const WebXVersion & other) :
        major(other.major),
        minor(other.minor),
        patch(other.patch) {}
    ~WebXVersion() {}

    std::string versionString() const {
        std::ostringstream oss;
        oss << major << "." << minor << "." << patch;
        return oss.str();
    }

private:
    struct VersionParts {
        uint32_t major;
        uint32_t minor;
        uint32_t patch;
    };

    static VersionParts parseAll(const std::string& versionStr) {
        std::vector<std::string> parts = split(versionStr, '.');
        if (parts.size() != 3) {
            spdlog::warn("Invalid version string '{}': expected 3 components", versionStr);
            return {0, 0, 0};
        }

        try {
            size_t pos;
            int maj = std::stoi(parts[0], &pos);
            if (pos != parts[0].size() || maj < 0) {
                throw std::invalid_argument("Invalid major");
            }
            int min = std::stoi(parts[1], &pos);
            if (pos != parts[1].size() || min < 0) {
                throw std::invalid_argument("Invalid minor");
            }
            int pat = std::stoi(parts[2], &pos);
            if (pos != parts[2].size() || pat < 0) {
                throw std::invalid_argument("Invalid patch");
            }

            return {
                static_cast<uint32_t>(maj),
                static_cast<uint32_t>(min),
                static_cast<uint32_t>(pat)
            };
        } catch (const std::exception& e) {
            spdlog::warn("Failed to parse version string '{:s}'", versionStr);
            return {0, 0, 0};
        }
    }

    static std::vector<std::string> split(const std::string & s, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }

public:
    const uint32_t major;
    const uint32_t minor;
    const uint32_t patch;    
};


#endif /* WEBX_VERSION_H */