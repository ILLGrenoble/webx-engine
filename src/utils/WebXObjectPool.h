#ifndef WEBX_OBJECT_POOL_H
#define WEBX_OBJECT_POOL_H

#include <tinythread/tinythread.h>
#include <memory>
#include <list>

template <typename T>
class WebXObjectPool {
public:
    WebXObjectPool() {
        this->init(10);
    }
    WebXObjectPool(long size) {
        this->init(size);
    }
    virtual ~WebXObjectPool() {}

    std::shared_ptr<T> get() {
        std::shared_ptr<T> object;
        {
            tthread::lock_guard<tthread::mutex> lock(_poolMutex);
            if (this->_pool.empty()) {
                object = std::make_shared<T>();

            } else {
              object = this->_pool.front();
              this->_pool.pop_front();
            }
        }

        // Automatically collect object
        return std::shared_ptr<T>(object.get(), [=](T*){
            this->put(object);
        }); 
    }

    // std::shared_ptr<T> getWithValues(_Args args...) {
    //     auto object = this->get();
    //     object->setValues(args);

    //     return object;
    // }

private:
    void init(long size) {
        tthread::lock_guard<tthread::mutex> lock(_poolMutex);
        for (long i = 0; i < size; i++) {
          this->_pool.push_back(std::make_shared<T>());
        }
    }

    void put(std::shared_ptr<T> object) {
        tthread::lock_guard<tthread::mutex> lock(_poolMutex);
    }

private:
    std::list<std::shared_ptr<T>> _pool;
    tthread::mutex _poolMutex;
};


#endif /* WEBX_OBJECT_POOL_H */