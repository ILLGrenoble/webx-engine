#ifndef WEBX_WINDOWS_RESPONSE_H
#define WEBX_WINDOWS_RESPONSE_H

#include <vector>
#include "WebXClientConnectorResponse.h"
#include <display/WebXWindowProperties.h>

class WebXWindowsResponse : public WebXClientConnectorResponse {
public:
    WebXWindowsResponse(const std::vector<WebXWindowProperties> & windows) {
        this->_windows = nlohmann::json{};
        for (std::vector<WebXWindowProperties>::const_iterator it = windows.begin(); it != windows.end(); it++) {
            const WebXWindowProperties & window = *it;
            const WebXRectangle & rectangle = window.rectangle;
            _windows.push_back({
            {"id", window.id}, 
            {"rectangle", {
                {"x", rectangle.x},
                {"y", rectangle.y},
                {"width", rectangle.width},
                {"height", rectangle.height}
            }}});
        }
    }
    virtual ~WebXWindowsResponse() {}

    virtual const nlohmann::json & getJson() const {
        return this->_windows;
    }

private:
    nlohmann::json _windows;
};


#endif /* WEBX_WINDOWS_RESPONSE_H */