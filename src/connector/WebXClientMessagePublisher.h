#ifndef WEBX_CLIENT_MESSAGE_PUBLISHER_H
#define WEBX_CLIENT_MESSAGE_PUBLISHER_H

#include <display/WebXConnection.h>
#include <utils/WebXQueue.h>
#include <tinythread/tinythread.h>

class WebXMessage;
namespace zmq {
class context_t;
}

class WebXClientMessagePublisher : public WebXConnection {
public:
    WebXClientMessagePublisher();
    virtual ~WebXClientMessagePublisher();

    void run(zmq::context_t * context, int port);
    void stop();

public:
    virtual void onDisplayChanged(const std::vector<WebXWindowProperties> & windows);

private:
    static void threadMain(void * arg);
    void mainLoop();

private:
    tthread::thread * _thread;
    tthread::mutex _mutex;
    bool _running;
    WebXQueue<WebXMessage> * _messageQueue;

    zmq::context_t * _context;
    int _port;
};


#endif /* WEBX_CLIENT_MESSAGE_PUBLISHER_H */