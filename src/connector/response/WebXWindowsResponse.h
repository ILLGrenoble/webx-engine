#ifndef WEBX_WINDOWS_RESPONSE_H
#define WEBX_WINDOWS_RESPONSE_H

#include <vector>
#include "WebXResponse.h"
#include <display/WebXWindowProperties.h>

class WebXWindowsResponse : public WebXResponse {
public:
    WebXWindowsResponse(const std::vector<WebXWindowProperties> & windows) :
        _windows(windows) {
    }
    virtual ~WebXWindowsResponse() {}

    virtual void toJson(nlohmann::json & j) const {
        j = nlohmann::json();
        j["windows"] = {};
        for (std::vector<WebXWindowProperties>::const_iterator it = this->_windows.begin(); it != this->_windows.end(); it++) {
            const WebXWindowProperties & window = *it;
            j["windows"].push_back({
            {"id", window.id},
            {"x", window.x},
            {"y", window.y},
            {"width", window.width},
            {"height", window.height}
            });
        }
    }

private:
    std::vector<WebXWindowProperties> _windows;
};


#endif /* WEBX_WINDOWS_RESPONSE_H */