#ifndef WEBX_POSITON_H
#define WEBX_POSITON_H

/* 
 * Represents a 2D position with x and y coordinates.
 */
class WebXPosition {
public:
    /* 
     * Default constructor initializes the position to (0, 0).
     */
    WebXPosition() :
        _x(0),
        _y(0) {}

    /* 
     * Parameterized constructor initializes the position to (x, y).
     */
    WebXPosition(int x, int y) :
        _x(x),
        _y(y) {}

    /* 
     * Virtual destructor for safe inheritance.
     */
    virtual ~WebXPosition() {}

    /* 
     * Equality operator to compare two WebXPosition objects.
     */
    bool operator==(const WebXPosition & position) {
        return (this == &position || (this->_x == position._x && this->_y == position._y));
    }

    /* 
     * Inequality operator to compare two WebXPosition objects.
     */
    bool operator!=(const WebXPosition & position) {
        return !this->operator==(position);
    }

    /* 
     * Getter for the x-coordinate.
     */
    int x() const {
        return this->_x;
    }

    /* 
     * Getter for the y-coordinate.
     */
    int y() const {
        return this->_y;
    }

private:    
    const int _x; /* The x-coordinate of the position. */
    const int _y; /* The y-coordinate of the position. */
};

#endif /* WEBX_POSITON_H */