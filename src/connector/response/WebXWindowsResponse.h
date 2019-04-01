#ifndef WEBX_WINDOWS_RESPONSE_H
#define WEBX_WINDOWS_RESPONSE_H

#include <vector>
#include "WebXClientConnectorResponse.h"

class WebXWindow;

class WebXWindowsResponse : public WebXClientConnectorResponse {
public:
    WebXWindowsResponse(const std::vector<WebXWindow *> & windows) :
        _windows(windows) {}
    virtual ~WebXWindowsResponse() {}

    virtual void toJson(nlohmann::json& j) const;

private:
    std::vector<WebXWindow *> _windows;
};


#endif /* WEBX_WINDOWS_RESPONSE_H */