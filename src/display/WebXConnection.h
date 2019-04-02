#ifndef WEBX_CONNECTION_H
#define WEBX_CONNECTION_H

#include <vector>
#include "WebXWindowProperties.h"

class WebXConnection {
public:
    WebXConnection() {}
    virtual ~WebXConnection() {}

    virtual void onDisplayChanged(const std::vector<WebXWindowProperties> & windows) = 0;
};


#endif /* WEBX_CONNECTION_H */