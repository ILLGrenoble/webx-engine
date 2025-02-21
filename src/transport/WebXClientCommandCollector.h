#ifndef WEBX_CLIENT_COMMAND_COLLECTOR_H
#define WEBX_CLIENT_COMMAND_COLLECTOR_H

#include <serializer/WebXBinarySerializer.h>
#include <gateway/WebXGateway.h>
#include <utils/WebXSettings.h>
#include <thread>
#include <mutex>
#include <string>

class WebXGateway;
class WebXInstruction;

namespace zmq {
class context_t;
class socket_t;
}

class WebXClientCommandCollector {
public:
    WebXClientCommandCollector(const WebXTransportSettings & settings, WebXGateway & gateway, const WebXBinarySerializer & serializer);
    virtual ~WebXClientCommandCollector();

    void run(zmq::context_t * context, const std::string & clientAddr, bool bindToClientAddr);
    void stop();

private:
    void mainLoop();
    zmq::socket_t createClientInstructionSubscriber();
    zmq::socket_t createEventBusSubscriber();

private:
    std::thread * _thread;

    WebXGateway & _gateway;
    const WebXBinarySerializer & _serializer;
    zmq::context_t * _context;
    std::string _clientAddr;
    bool _bindToClientAddr;
    std::string _eventBusAddr;
    unsigned char _sessionId[16];
};


#endif /* WEBX_CLIENT_COMMAND_COLLECTOR_H */
