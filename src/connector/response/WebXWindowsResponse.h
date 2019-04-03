#ifndef WEBX_WINDOWS_RESPONSE_H
#define WEBX_WINDOWS_RESPONSE_H

#include <vector>
#include "WebXClientConnectorResponse.h"
#include <display/WebXWindowProperties.h>

class WebXWindowsResponse : public WebXClientConnectorResponse {
public:
    WebXWindowsResponse(const std::vector<WebXWindowProperties> & windows) :
        _windows(windows) {
    }
    virtual ~WebXWindowsResponse() {}

    virtual void toJson(nlohmann::json & j) const {
        j = nlohmann::json();
        for (std::vector<WebXWindowProperties>::const_iterator it = this->_windows.begin(); it != this->_windows.end(); it++) {
            const WebXWindowProperties & window = *it;
            const WebXRectangle & rectangle = window.rectangle;
            j.push_back({
            {"id", window.id},
            {"bpp", window.bpp},
            {"rectangle", {
                {"x", rectangle.x},
                {"y", rectangle.y},
                {"width", rectangle.width},
                {"height", rectangle.height}
            }}});
        }
    }

private:
    std::vector<WebXWindowProperties> _windows;
};


#endif /* WEBX_WINDOWS_RESPONSE_H */