#ifndef WEBX_CLIENT_H
#define WEBX_CLIENT_H

#include <memory>

class WebXClient {

public:
    WebXClient();
    virtual ~WebXClient();

private:
    uint64_t _index;
    uint32_t _id;

};


#endif /* WEBX_CLIENT_H */