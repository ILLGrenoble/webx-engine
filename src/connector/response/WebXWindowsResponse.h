#ifndef WEBX_WINDOWS_RESPONSE_H
#define WEBX_WINDOWS_RESPONSE_H

#include <vector>
#include "WebXClientConnectorResponse.h"
#include <display/WebXWindow.h>

class WebXWindowsResponse : public WebXClientConnectorResponse {
public:
    WebXWindowsResponse(const std::vector<WebXWindow *> & windows) {
        this->_windows = nlohmann::json{};
        for (std::vector<WebXWindow *>::const_iterator it = windows.begin(); it != windows.end(); it++) {
            WebXWindow * window = *it;
            const WebXRectangle & rectangle = window->getRectangle();
            _windows.push_back({
            {"id", window->getX11Window()}, 
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