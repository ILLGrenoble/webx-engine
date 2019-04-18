#ifndef WEBX_CLIENT_MESSAGE_PUBLISHER_H
#define WEBX_CLIENT_MESSAGE_PUBLISHER_H

#include <display/WebXConnection.h>
#include <utils/WebXQueue.h>
#include <tinythread/tinythread.h>

class WebXMessage;
class WebXSerializer;
namespace zmq {
class context_t;
}

class WebXClientMessagePublisher : public WebXConnection {
public:
    WebXClientMessagePublisher();
    virtual ~WebXClientMessagePublisher();

    void run(WebXSerializer * serializer, zmq::context_t * context, int port);
    void stop();

public:
    virtual void onDisplayChanged(const std::vector<WebXWindowProperties> & windows);
    virtual void onImageChanged(unsigned long windowId, std::shared_ptr<WebXImage> image);
    virtual void onSubImagesChanged(unsigned long windowId, std::vector<WebXSubImage> subImages);

private:
    static void threadMain(void * arg);
    void mainLoop();

private:
    tthread::thread * _thread;
    tthread::mutex _mutex;
    bool _running;
    WebXQueue<WebXMessage> * _messageQueue;

    WebXSerializer * _serializer;
    zmq::context_t * _context;
    int _port;
};


#endif /* WEBX_CLIENT_MESSAGE_PUBLISHER_H */