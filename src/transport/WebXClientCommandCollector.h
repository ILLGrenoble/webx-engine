#ifndef WEBX_CLIENT_COMMAND_COLLECTOR_H
#define WEBX_CLIENT_COMMAND_COLLECTOR_H

#include <thread>
#include <mutex>

class WebXSettings;
class WebXInstruction;
class WebXBinarySerializer;
namespace zmq {
class context_t;
class socket_t;
}

class WebXClientCommandCollector {
public:
    WebXClientCommandCollector();
    virtual ~WebXClientCommandCollector();

    void run(WebXBinarySerializer * serializer, zmq::context_t * context, const std::string & clientAddr, bool bindToClientAddr, const std::string & eventBusAddr, WebXSettings * settings);
    void stop();

private:
    void mainLoop();
    zmq::socket_t createClientInstructionSubscriber();
    zmq::socket_t createEventBusSubscriber();

private:
    std::thread * _thread;

    WebXBinarySerializer * _serializer;
    zmq::context_t * _context;
    std::string _clientAddr;
    bool _bindToClientAddr;
    std::string _eventBusAddr;
    unsigned char _sessionId[16];
};


#endif /* WEBX_CLIENT_COMMAND_COLLECTOR_H */
