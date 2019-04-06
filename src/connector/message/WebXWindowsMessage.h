#ifndef WEBX_WINDOWS_MESSAGE_H
#define WEBX_WINDOWS_MESSAGE_H

#include <vector>
#include "WebXMessage.h"
#include <display/WebXWindowProperties.h>

class WebXWindowsMessage : public WebXMessage {
public:
    WebXWindowsMessage(const std::vector<WebXWindowProperties> & windows) :
        _windows(windows) {}
    virtual ~WebXWindowsMessage() {}

    virtual void toJson(nlohmann::json & j) const {
        j = nlohmann::json();
        j["windows"] = {};
        for (std::vector<WebXWindowProperties>::const_iterator it = this->_windows.begin(); it != this->_windows.end(); it++) {
            const WebXWindowProperties & window = *it;
            j["windows"].push_back({
            {"id", window.id}, 
            {"bpp", window.bpp}, 
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


#endif /* WEBX_WINDOWS_RESMESSAGE*/