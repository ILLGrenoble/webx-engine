#ifndef WEBX_BINARY_MESSAGE_SERIALIZER_H
#define WEBX_BINARY_MESSAGE_SERIALIZER_H

#include <zmq.hpp>
#include <memory>

template <typename T>
class WebXBinaryMessageSerializer {
public:
    virtual ~WebXBinaryMessageSerializer() {}
    virtual zmq::message_t * serialize(std::shared_ptr<T> message) = 0;
};


#endif //WEBX_BINARY_MESSAGE_SERIALIZER_H
