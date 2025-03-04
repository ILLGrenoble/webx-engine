#ifndef WEBX_POSITON_H
#define WEBX_POSITON_H

class WebXPosition {
public:
    WebXPosition() :
        _x(0),
        _y(0) {}
    WebXPosition(int x, int y) :
        _x(x),
        _y(y) {}
    virtual ~WebXPosition() {}

    bool operator==(const WebXPosition & position) {
        return (this == &position || (this->_x == position._x && this->_y == position._y));
    }

    bool operator!=(const WebXPosition & position) {
        return !this->operator==(position);
    }

    int x() const {
        return this->_x;
    }

    int y() const {
        return this->_y;
    }

private:    
    int _x;
    int _y;
};


#endif /* WEBX_POSITON_H */