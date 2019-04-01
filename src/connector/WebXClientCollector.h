#ifndef WEBX_CLIENT_COLLECTOR_H
#define WEBX_CLIENT_COLLECTOR_H

#include <utils/WebXQueue.h>
#include <tinythread/tinythread.h>

class WebXMessage;

class WebXClientCollector {
public:
    WebXClientCollector();
    virtual ~WebXClientCollector();

    void run();
    void stop();

private:
    static void threadMain(void * arg);
    void mainLoop();

private:
    tthread::thread * _thread;
    tthread::mutex _mutex;
    bool _running;
    WebXQueue<WebXMessage> * _messageQueue;
};


#endif /* WEBX_CLIENT_COLLECTOR_H */