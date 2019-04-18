#ifndef WEBX_WINDOWS_MESSAGE_H
#define WEBX_WINDOWS_MESSAGE_H

#include <vector>
#include "WebXMessage.h"
#include <display/WebXWindowProperties.h>

class WebXWindowsMessage : public WebXMessage {
public:
    WebXWindowsMessage(const std::vector<WebXWindowProperties> & windows) :
        WebXMessage("windows"),
        windows(windows) {}
    virtual ~WebXWindowsMessage() {}

    std::vector<WebXWindowProperties> windows;
};


#endif /* WEBX_WINDOWS_MESSAGE_H*/