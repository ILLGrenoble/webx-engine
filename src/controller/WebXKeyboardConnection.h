#ifndef WEBX_KEYBOARD_CONNECTION_H
#define WEBX_KEYBOARD_CONNECTION_H

#include <thread>
#include <mutex>
#include <vector>

class WebXWindow;

class WebXKeyboardConnection {
public:
    WebXKeyboardConnection();
    virtual ~WebXKeyboardConnection();

    void run();
    void stop();

private:
    void mainLoop();
    void printWindows();
    void exportWindowImages();

private:
    unsigned long _id;

    std::thread * _thread;
    std::mutex _mutex;
    bool _running;
};


#endif /* WEBX_KEYBOARD_CONNECTION_H */