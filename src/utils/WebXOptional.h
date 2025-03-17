#ifndef WEBX_OPTIONAL_H
#define WEBX_OPTIONAL_H

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

    static const WebXOptional Value(const ItemType & data) {
        return WebXOptional(data);
    };
    static const WebXOptional Empty() {
        return WebXOptional();
    }

    WebXOptional & operator=(const WebXOptional & optional) {
        if (this != &optional) {
            this->_value = optional._value;
            this->_hasValue = optional._hasValue;
        }
        return *this;
    }

    const ItemType & value() const {
        if (!this->_hasValue) {
            throw std::runtime_error("Optional has no value");
        }
        return this->_value;
    }

    const ItemType & orElse(const ItemType & elseValue) const {
        if (!this->_hasValue) {
            return elseValue;
        }
        return this->_value;
    }

    const bool hasValue() const {
        return this->_hasValue;
    }

private:    
    ItemType _value;
    bool _hasValue;
};

#endif /* WEBX_OPTIONAL_H */