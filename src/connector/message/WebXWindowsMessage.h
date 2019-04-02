#ifndef WEBX_WINDOWS_MESSAGE_H
#define WEBX_WINDOWS_MESSAGE_H

#include <vector>
#include "WebXMessage.h"
#include <display/WebXWindowProperties.h>

class WebXWindowsMessage : public WebXMessage {
public:
    WebXWindowsMessage(const std::vector<WebXWindowProperties> & windows) {
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
    virtual ~WebXWindowsMessage() {}

    virtual const nlohmann::json & getJson() const {
        return this->_windows;
    }

private:
    nlohmann::json _windows;
};


#endif /* WEBX_WINDOWS_RESMESSAGE*/