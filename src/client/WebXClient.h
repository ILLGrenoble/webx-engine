#ifndef WEBX_CLIENT_H
#define WEBX_CLIENT_H

#include <memory>
#include <chrono>

class WebXClient {
public:
    enum PingStatus {
        WaitingToPing = 0,
        RequiresPing,
        WaitingForPong,
        PongTimeout,
    };

public:
    WebXClient(uint32_t id, uint64_t index);
    virtual ~WebXClient();

    uint32_t getId() const {
        return this->_id;
    }

    uint64_t getIndex() const {
        return this->_index;
    }

    void updatePingStatus() {
        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        if (this->_pingStatus == WaitingToPing) {
            std::chrono::duration<double, std::milli> timeSincePong = now - this->_pongReceivedTime;
            this->_pingStatus = timeSincePong.count() > PING_WAIT_INTERVAL_MS ? RequiresPing : WaitingToPing;
        
        } else if (this->_pingStatus == WaitingForPong) {
            std::chrono::duration<double, std::milli> timeSincePing = now - this->_pingSentTime;
            this->_pingStatus = timeSincePing.count() > PONG_RESPONSE_TIMEOUT_MS ? PongTimeout : WaitingForPong;
        }
    }

    PingStatus getPingStatus() const {
        return this->_pingStatus;
    }

    void onPingSent() {
        this->_pingSentTime = std::chrono::high_resolution_clock::now();
        this->_pingStatus = WaitingForPong;
    }

    void onPongReceived() {
        this->_pongReceivedTime = std::chrono::high_resolution_clock::now();
        this->_pingStatus = WaitingToPing;
    }

private:
    const static int PING_WAIT_INTERVAL_MS = 5000;
    const static int PONG_RESPONSE_TIMEOUT_MS = 10000;

    uint32_t _id;
    uint64_t _index;

    PingStatus _pingStatus;
    std::chrono::high_resolution_clock::time_point _pingSentTime;
    std::chrono::high_resolution_clock::time_point _pongReceivedTime;

};


#endif /* WEBX_CLIENT_H */