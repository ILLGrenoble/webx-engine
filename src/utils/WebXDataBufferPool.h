#ifndef WEBX_OBJECT_POOL_H
#define WEBX_OBJECT_POOL_H

#include <tinythread/tinythread.h>
#include <memory>
#include <list>
#include "WebXDataBuffer.h"

class WebXDataBufferPool {
public:
    WebXDataBufferPool() {
        this->init(10, 1024);
    }
    WebXDataBufferPool(long poolSize) {
        this->init(poolSize, 1024);
    }
    WebXDataBufferPool(long poolSize, long initialBufferCapacity) {
        this->init(poolSize, 1024);
    }
    virtual ~WebXDataBufferPool() {}

    std::shared_ptr<WebXDataBuffer> get(long length) {
        std::shared_ptr<WebXDataBuffer> dataBuffer;
        {
            tthread::lock_guard<tthread::mutex> lock(_poolMutex);
            if (this->_pool.empty()) {
                dataBuffer = std::make_shared<WebXDataBuffer>(length);

            } else {
                // Find first dataBuffer with capacity large enough (or first one otherwise)
                auto it = std::find_if(this->_pool.begin(), this->_pool.end(), [length](const std::shared_ptr<WebXDataBuffer> & aDataBuffer) {return aDataBuffer->getCapacity() >= length;});
                it = (it == this->_pool.end()) ? this->_pool.begin() : it;
                dataBuffer = *it;
                this->_pool.erase(it);
            }
        }

        // Automatically collect dataBuffer
        return std::shared_ptr<WebXDataBuffer>(dataBuffer.get(), [=](WebXDataBuffer*) {
            dataBuffer->reset();
            this->put(dataBuffer);
        }); 
    }

private:
    void init(long poolSize, long initialBufferCapacity) {
        tthread::lock_guard<tthread::mutex> lock(_poolMutex);
        for (long i = 0; i < poolSize; i++) {
          this->_pool.push_back(std::make_shared<WebXDataBuffer>(initialBufferCapacity));
        }
    }

    void put(std::shared_ptr<WebXDataBuffer> dataBuffer) {
        tthread::lock_guard<tthread::mutex> lock(_poolMutex);
        this->_pool.push_back(dataBuffer);
    }

private:
    std::list<std::shared_ptr<WebXDataBuffer>> _pool;
    tthread::mutex _poolMutex;
};


#endif /* WEBX_OBJECT_POOL_H */