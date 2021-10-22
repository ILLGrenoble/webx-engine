#ifndef WEBX_CLIENT_COMMAND_COLLECTOR_H
#define WEBX_CLIENT_COMMAND_COLLECTOR_H

#include <tinythread/tinythread.h>

class WebXInstruction;
class WebXBinarySerializer;
namespace zmq {
class context_t;
}

class WebXClientCommandCollector {
public:
    WebXClientCommandCollector();
    virtual ~WebXClientCommandCollector();

    void run(WebXBinarySerializer * serializer, zmq::context_t * context, int port);
    void stop();

private:
    static void threadMain(void * arg);
    void mainLoop();

private:
    tthread::thread * _thread;
    tthread::mutex _mutex;
    bool _running;

    WebXBinarySerializer * _serializer;
    zmq::context_t * _context;
    int _port;
};


#endif /* WEBX_CLIENT_COMMAND_COLLECTOR_H */
