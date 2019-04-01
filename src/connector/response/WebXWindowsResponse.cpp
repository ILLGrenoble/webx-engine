#include "WebXWindowsResponse.h"
#include <display/WebXWindow.h>

void WebXWindowsResponse::toJson(nlohmann::json& j) const {
    j = nlohmann::json();
    for (std::vector<WebXWindow *>::const_iterator it = this->_windows.begin(); it != this->_windows.end(); it++) {
        WebXWindow * window = *it;
        const WebXRectangle & rectangle = window->getRectangle();
        j.push_back({
            {"id", window->getX11Window()}, 
            {"rectangle", {
                {"x", rectangle.x},
                {"y", rectangle.y},
                {"width", rectangle.width},
                {"height", rectangle.height}
            }}});
    }
}