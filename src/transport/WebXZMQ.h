#include <zmq.hpp>

#if !defined(CPPZMQ_VERSION) || (CPPZMQ_VERSION < ZMQ_MAKE_VERSION(4, 3, 1))
#define COMPILE_FOR_CPPZMQ_BEFORE_4_3_1
#endif