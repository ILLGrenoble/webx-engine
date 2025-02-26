#ifndef WEBX_CLIENT_GROUP_H
#define WEBX_CLIENT_GROUP_H

#include <vector>
#include <memory>

class WebXClient;

class WebXClientGroup {

public:
    WebXClientGroup();
    virtual ~WebXClientGroup();

private:
    std::vector<std::shared_ptr<WebXClient>> _clients;

};


#endif /* WEBX_CLIENT_GROUP_H */