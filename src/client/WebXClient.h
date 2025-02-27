#ifndef WEBX_CLIENT_H
#define WEBX_CLIENT_H

#include <memory>

class WebXClient {

public:
    WebXClient(uint32_t id, uint64_t index);
    virtual ~WebXClient();

    uint32_t getId() const {
        return this->_id;
    }

    uint64_t getIndex() const {
        return this->_index;
    }

private:
    uint32_t _id;
    uint64_t _index;

};


#endif /* WEBX_CLIENT_H */