#ifndef WEBX_RESULT_H
#define WEBX_RESULT_H

#include <string>

template<typename ItemType>
class WebXResult {

private:
    WebXResult(const ItemType data, bool ok) : 
        _data(data),
        _error(),
        _ok(true) {
    }
    WebXResult(const std::string & error, bool ok) :
        _data(),
        _error(error),
        _ok(false) {
    }

public:
    static const WebXResult Ok(const ItemType & data) {
        return WebXResult(data, true);
    };
    static const WebXResult Err(const std::string & error) {
        return WebXResult(error, false);
    }

    virtual ~WebXResult() {}

    const ItemType & data() const {
        return this->_data;
    }

    const std::string & error() const {
        return this->_error;
    }

    const bool ok() const {
        return this->_ok;
    }

private:    
    const ItemType _data;
    const std::string _error;
    const bool _ok;
};


// Specialization for ItemType = std::string
template<>
class WebXResult<std::string> {
private:
    WebXResult(const std::string & value, bool ok) :
        _data(ok ? value : ""),
        _error(ok ? "" : value),
        _ok(ok) {
    }

public:
    static WebXResult Ok(const std::string& data) { return WebXResult(data, true); }
    static WebXResult Err(const std::string& error) { return WebXResult(error, false); }

    virtual ~WebXResult() {}

    const std::string & data() const {
        return this->_data;
    }

    const std::string & error() const {
        return this->_error;
    }

    const bool ok() const {
        return this->_ok;
    }

private:    
    const std::string _data;
    const std::string _error;
    const bool _ok;
};


#endif /* WEBX_RESULT_H */