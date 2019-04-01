#ifndef WEBX_CONNECTION_H
#define WEBX_CONNECTION_H

#include <vector>

class WebXWindow;

class WebXConnection {
public:
    WebXConnection() {}
    virtual ~WebXConnection() {}

    virtual void onDisplayChanged(const std::vector<WebXWindow *> & windows) = 0;
};


#endif /* WEBX_CONNECTION_H */