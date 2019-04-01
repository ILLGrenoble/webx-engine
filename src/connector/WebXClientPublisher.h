#ifndef WEBX_CLIENT_PUBLISHER_H
#define WEBX_CLIENT_PUBLISHER_H

#include <display/WebXConnection.h>
#include <utils/WebXQueue.h>
#include <tinythread/tinythread.h>

class WebXMessage;

class WebXClientPublisher : public WebXConnection {
public:
    WebXClientPublisher();
    virtual ~WebXClientPublisher();

    void run();
    void stop();

public:
    virtual void onDisplayChanged(const std::vector<WebXWindow *> & windows);

private:
    static void threadMain(void * arg);
    void mainLoop();

private:
    tthread::thread * _thread;
    tthread::mutex _mutex;
    bool _running;
    WebXQueue<WebXMessage> * _messageQueue;
};


#endif /* WEBX_CLIENT_PUBLISHER_H */