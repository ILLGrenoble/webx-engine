#ifndef WEBX_CLIENT_COMMAND_COLLECTOR_H
#define WEBX_CLIENT_COMMAND_COLLECTOR_H

#include <thread>
#include <mutex>
#include <string>

class WebXGateway;
class WebXSettings;
class WebXInstruction;
class WebXBinarySerializer;
namespace zmq {
class context_t;
class socket_t;
}

class WebXClientCommandCollector {
public:
    WebXClientCommandCollector(WebXGateway * gateway, WebXBinarySerializer * serializer);
    virtual ~WebXClientCommandCollector();

    void run(zmq::context_t * context, const std::string & clientAddr, bool bindToClientAddr, const std::string & eventBusAddr, WebXSettings * settings);
    void stop();

private:
    void mainLoop();
    zmq::socket_t createClientInstructionSubscriber();
    zmq::socket_t createEventBusSubscriber();

private:
    std::thread * _thread;

    WebXGateway * _gateway;
    WebXBinarySerializer * _serializer;
    zmq::context_t * _context;
    std::string _clientAddr;
    bool _bindToClientAddr;
    std::string _eventBusAddr;
    unsigned char _sessionId[16];
};


#endif /* WEBX_CLIENT_COMMAND_COLLECTOR_H */
