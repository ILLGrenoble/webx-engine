#ifndef WEBX_CLIENT_COMMAND_COLLECTOR_H
#define WEBX_CLIENT_COMMAND_COLLECTOR_H

#include <utils/WebXQueue.h>
#include <tinythread/tinythread.h>

class WebXInstruction;
namespace zmq {
class context_t;
}

class WebXClientCommandCollector {
public:
    WebXClientCommandCollector();
    virtual ~WebXClientCommandCollector();

    void run(zmq::context_t * context, int port);
    void stop();

private:
    static void threadMain(void * arg);
    void mainLoop();

private:
    tthread::thread * _thread;
    tthread::mutex _mutex;
    bool _running;
    WebXQueue<WebXInstruction> * _commandQueue;

    zmq::context_t * _context;
    int _port;
};


#endif /* WEBX_CLIENT_COMMAND_COLLECTOR_H */