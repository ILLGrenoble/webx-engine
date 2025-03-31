#ifndef WEBX_RESULT_H
#define WEBX_RESULT_H

#include <string>

/**
 * A generic result class that encapsulates either a value or an error.
 * 
 * @tparam ItemType The type of the value.
 */
template<typename ItemType>
class WebXResult {

private:
    WebXResult(const ItemType data) : 
        _data(data),
        _error(),
        _ok(true) {
    }
    WebXResult(const std::string & error) :
        _data(),
        _error(error),
        _ok(false) {
    }

public:
    /**
     * Creates a successful result containing the given data.
     * 
     * @param data The value to encapsulate.
     * @return A WebXResult instance representing success.
     */
    static const WebXResult Ok(const ItemType & data) {
        return WebXResult(data);
    };

    /**
     * Creates an error result containing the given error message.
     * 
     * @param error The error message to encapsulate.
     * @return A WebXResult instance representing an error.
     */
    static const WebXResult Err(const std::string & error) {
        return WebXResult(error);
    }

    /**
     * Destructor for WebXResult.
     */
    virtual ~WebXResult() {}

    /**
     * Retrieves the encapsulated value.
     * 
     * @return The value if the result is successful.
     * @throws Undefined behavior if the result is an error.
     */
    const ItemType & data() const {
        return this->_data;
    }

    /**
     * Retrieves the encapsulated error message.
     * 
     * @return The error message if the result is an error.
     * @throws Undefined behavior if the result is successful.
     */
    const std::string & error() const {
        return this->_error;
    }

    /**
     * Checks if the result represents success.
     * 
     * @return True if the result is successful, false otherwise.
     */
    const bool ok() const {
        return this->_ok;
    }

private:    
    const ItemType _data;
    const std::string _error;
    const bool _ok;
};


/**
 * Specialization of WebXResult for std::string.
 * Encapsulates either a string value or an error message.
 */
template<>
class WebXResult<std::string> {
private:
    WebXResult(const std::string & value, bool ok) :
        _data(ok ? value : ""),
        _error(ok ? "" : value),
        _ok(ok) {
    }

public:
    /**
     * Creates a successful result containing the given string.
     * 
     * @param data The string value to encapsulate.
     * @return A WebXResult instance representing success.
     */
    static WebXResult Ok(const std::string& data) { return WebXResult(data, true); }

    /**
     * Creates an error result containing the given error message.
     * 
     * @param error The error message to encapsulate.
     * @return A WebXResult instance representing an error.
     */
    static WebXResult Err(const std::string& error) { return WebXResult(error, false); }

    /**
     * Destructor for WebXResult.
     */
    virtual ~WebXResult() {}

    /**
     * Retrieves the encapsulated string value.
     * 
     * @return The string value if the result is successful.
     * @throws Undefined behavior if the result is an error.
     */
    const std::string & data() const {
        return this->_data;
    }

    /**
     * Retrieves the encapsulated error message.
     * 
     * @return The error message if the result is an error.
     * @throws Undefined behavior if the result is successful.
     */
    const std::string & error() const {
        return this->_error;
    }

    /**
     * Checks if the result represents success.
     * 
     * @return True if the result is successful, false otherwise.
     */
    const bool ok() const {
        return this->_ok;
    }

private:    
    const std::string _data;
    const std::string _error;
    const bool _ok;
};

/**
 * Specialization of WebXResult for void.
 * Represents a result with no value, only success or error.
 */
template<>
class WebXResult<void> {
private:
    WebXResult() :
        _error(""),
        _ok(true) {
    }
    WebXResult(const std::string & error) :
        _error(error),
        _ok(false) {
    }

public:
    /**
     * Creates a successful result with no value.
     * 
     * @return A WebXResult instance representing success.
     */
    static WebXResult Ok() { return WebXResult(); }

    /**
     * Creates an error result containing the given error message.
     * 
     * @param error The error message to encapsulate.
     * @return A WebXResult instance representing an error.
     */
    static WebXResult Err(const std::string& error) { return WebXResult(error); }

    /**
     * Destructor for WebXResult.
     */
    virtual ~WebXResult() {}

    /**
     * Retrieves the encapsulated error message.
     * 
     * @return The error message if the result is an error.
     * @throws Undefined behavior if the result is successful.
     */
    const std::string & error() const {
        return this->_error;
    }

    /**
     * Checks if the result represents success.
     * 
     * @return True if the result is successful, false otherwise.
     */
    const bool ok() const {
        return this->_ok;
    }

private:    
    const std::string _error;
    const bool _ok;
};


#endif /* WEBX_RESULT_H */