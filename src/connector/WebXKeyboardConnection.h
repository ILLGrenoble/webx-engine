#ifndef WEBX_KEYBOARD_CONNECTION_H
#define WEBX_KEYBOARD_CONNECTION_H

#include <tinythread/tinythread.h>
#include <vector>

class WebXWindow;

class WebXKeyboardConnection {
public:
    WebXKeyboardConnection();
    virtual ~WebXKeyboardConnection();

    void run();
    void stop();

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
};


#endif /* WEBX_KEYBOARD_CONNECTION_H */