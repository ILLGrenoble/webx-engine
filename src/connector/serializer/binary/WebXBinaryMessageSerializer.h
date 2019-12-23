#ifndef WEBX_BINARY_MESSAGE_SERIALIZER_H
#define WEBX_BINARY_MESSAGE_SERIALIZER_H

#include <zmq.hpp>

template <typename T>
class WebXBinaryMessageSerializer {
public:
    virtual ~WebXBinaryMessageSerializer() {}
    virtual zmq::message_t * serialize(const T * message) = 0;
};


#endif //WEBX_BINARY_MESSAGE_SERIALIZER_H
