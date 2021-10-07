#include <stdlib.h>
#include <zmq.hpp>
#include <unistd.h>

int main() {

    //  Prepare our context and socket
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
        printf("Sending %s\n", (const char *)zmqMessage.data());
        auto retVal = socket.send(zmqMessage, zmq::send_flags::none);

        usleep(10000);
    }

    printf("Closing socket...\n");
    socket.close();
    context.close();
    printf("... closed\n");

}