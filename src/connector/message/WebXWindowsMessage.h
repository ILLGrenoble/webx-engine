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
        for (std::vector<WebXWindowProperties>::const_iterator it = this->_windows.begin(); it != this->_windows.end(); it++) {
            const WebXWindowProperties & window = *it;
            const WebXRectangle & rectangle = window.rectangle;
            j.push_back({
            {"id", window.id}, 
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


#endif /* WEBX_WINDOWS_RESMESSAGE*/