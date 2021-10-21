#include <stdlib.h>
#include <zmq.hpp>
#include <unistd.h>
#include <tinythread/tinythread.h>


#if !defined(CPPZMQ_VERSION) || (CPPZMQ_VERSION < ZMQ_MAKE_VERSION(4, 3, 1))
#define COMPILE_FOR_CPPZMQ_BEFORE_4_3_1
#endif

void pusher(void * arg) {
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_PUSH);
    int linger = 0;
    socket.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));

    std::string address = "tcp://localhost:5555";
    printf("Connecting to socket \"%s\"...\n", address.c_str());
    socket.connect(address.c_str());
    printf("... connected\n");


    for (int i = 0; i < 10000; i++) {
        std::string message = "{\"type\":5,\"id\":257,\"x\":821.691368788143,\"y\":203.50043591979076,\"buttonMask\":0}";
        zmq::message_t zmqMessage(message.c_str(), message.size());
        printf("%d Sending %s\n", i, (const char *)zmqMessage.data());
#ifdef COMPILE_FOR_CPPZMQ_BEFORE_4_3_1
        socket.send(zmqMessage);
#else
        socket.send(zmqMessage, zmq::send_flags::none);
#endif

        usleep(10000);
    }

    printf("Closing socket...\n");
    socket.close();
    context.close();
    printf("... closed\n");
}

int main() {
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_PULL);
    int linger = 0;
    socket.setsockopt(ZMQ_LINGER, &linger, sizeof(linger));

    std::string address = "tcp://*:5555";
    socket.bind(address.c_str());

    tthread::thread * pusherThread = new tthread::thread(pusher, 0);


    while (true) {
        zmq::message_t instructionMessage;
#ifdef COMPILE_FOR_CPPZMQ_BEFORE_4_3_1
        bool recvResult = socket.recv(&instructionMessage);
#else
        auto recvResult = socket.recv(instructionMessage);
#endif
        if (recvResult) {
            const char * instructionData = (const char *)instructionMessage.data();
            std::string instructionString;
            instructionString.assign(instructionData, instructionMessage.size());
            printf("Received instruction %s\n", instructionString.c_str());
        }
    }
}
