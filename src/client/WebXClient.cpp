#include "WebXClient.h"

WebXClient::WebXClient(uint32_t id, uint64_t index) :
    _id(id),
    _index(index),
    _pingStatus(PingStatus::WaitingToPing),
    _pingSentTime(std::chrono::high_resolution_clock::now()),
    _pongReceivedTime(std::chrono::high_resolution_clock::now()) {
}

WebXClient::~WebXClient() {
    
}
