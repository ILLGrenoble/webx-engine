#ifndef WEBX_SIZE_H
#define WEBX_SIZE_H

class WebXSize {
public:
    WebXSize() :
        _width(0),
        _height(0) {}
    WebXSize(int width, int height) :
        _width(width),
        _height(height) {}
    WebXSize(const WebXSize & size) :
        _width(size._width),
        _height(size._height) {}
    virtual ~WebXSize() {}

    bool operator == (const WebXSize & size) const {
        return this->_width == size._width && this->_height == size._height;
    }

    bool operator != (const WebXSize & size) const {
        return !operator==(size);
    }

    int area() const {
        return this->_width * this->_height;
    }

    int width() const {
        return this->_width;
    }

    int height() const {
        return this->_height;
    }

private:
    int _width;
    int _height;
};


#endif /* WEBX_SIZE_H */