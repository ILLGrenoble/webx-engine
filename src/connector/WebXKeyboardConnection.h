#ifndef WEBX_KEYBOARD_CONNECTION_H
#define WEBX_KEYBOARD_CONNECTION_H

#include <display/WebXConnection.h>
#include <tinythread/tinythread.h>
#include <vector>

class WebXWindow;

class WebXKeyboardConnection : public WebXConnection {
public:
    WebXKeyboardConnection();
    virtual ~WebXKeyboardConnection();

    void run();
    void stop();

public:
    virtual void onDisplayChanged(const std::vector<WebXWindow *> & windows);

private:
    static void threadMain(void * arg);
    void mainLoop();
    void printWindows();
    void exportWindowImages();

private:
    unsigned long _id;

    tthread::thread * _thread;
    tthread::mutex _mutex;
    bool _running;

    std::vector<WebXWindow *> _windows;
};


#endif /* WEBX_KEYBOARD_CONNECTION_H */