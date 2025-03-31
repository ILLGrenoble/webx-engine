#ifndef WEBX_OPTIONAL_H
#define WEBX_OPTIONAL_H

/**
 * A class representing an optional value.
 * 
 * @tparam ItemType The type of the optional value.
 */
template<typename ItemType>
class WebXOptional {
private:
    WebXOptional(const ItemType value) : 
        _value(value),
        _hasValue(true) {
    }
    WebXOptional() :
        _hasValue(false) {
    }

public:
    virtual ~WebXOptional() {}

    /**
     * Creates a WebXOptional instance containing a value.
     * 
     * @param data The value to store in the optional.
     * @return A WebXOptional instance with the specified value.
     */
    static const WebXOptional Value(const ItemType & data) {
        return WebXOptional(data);
    };

    /**
     * Creates an empty WebXOptional instance.
     * 
     * @return A WebXOptional instance with no value.
     */
    static const WebXOptional Empty() {
        return WebXOptional();
    }

    /**
     * Assigns the value and state of another WebXOptional instance to this instance.
     * 
     * @param optional The WebXOptional instance to copy from.
     * @return A reference to this WebXOptional instance.
     */
    WebXOptional & operator=(const WebXOptional & optional) {
        if (this != &optional) {
            this->_value = optional._value;
            this->_hasValue = optional._hasValue;
        }
        return *this;
    }

    /**
     * Retrieves the value stored in the optional.
     * 
     * @throws std::runtime_error if the optional has no value.
     * @return The value stored in the optional.
     */
    const ItemType & value() const {
        if (!this->_hasValue) {
            throw std::runtime_error("Optional has no value");
        }
        return this->_value;
    }

    /**
     * Retrieves the value stored in the optional, or a default value if the optional is empty.
     * 
     * @param elseValue The default value to return if the optional is empty.
     * @return The value stored in the optional, or the default value.
     */
    const ItemType & orElse(const ItemType & elseValue) const {
        if (!this->_hasValue) {
            return elseValue;
        }
        return this->_value;
    }

    /**
     * Checks whether the optional contains a value.
     * 
     * @return True if the optional contains a value, false otherwise.
     */
    const bool hasValue() const {
        return this->_hasValue;
    }

private:    
    ItemType _value;
    bool _hasValue;
};

#endif /* WEBX_OPTIONAL_H */