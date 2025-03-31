#ifndef WEBX_SIZE_H
#define WEBX_SIZE_H

/*
 * Class representing a 2D size with width and height.
 */
class WebXSize {
public:
    /*
     * Default constructor initializing width and height to 0.
     */
    WebXSize() :
        _width(0),
        _height(0) {}

    /*
     * Constructor initializing width and height with given values.
     */
    WebXSize(int width, int height) :
        _width(width),
        _height(height) {}

    /*
     * Copy constructor.
     */
    WebXSize(const WebXSize & size) :
        _width(size._width),
        _height(size._height) {}

    /*
     * Virtual destructor.
     */
    virtual ~WebXSize() {}

    /*
     * Equality operator to compare two WebXSize objects.
     */
    bool operator==(const WebXSize & size) const {
        return this->_width == size._width && this->_height == size._height;
    }

    /*
     * Inequality operator to compare two WebXSize objects.
     */
    bool operator!=(const WebXSize & size) const {
        return !operator==(size);
    }

    /*
     * Method to calculate the area (width * height).
     */
    int area() const {
        return this->_width * this->_height;
    }

    /*
     * Getter for the width.
     */
    int width() const {
        return this->_width;
    }

    /*
     * Getter for the height.
     */
    int height() const {
        return this->_height;
    }

private:
    int _width;
    int _height;
};

#endif /* WEBX_SIZE_H */