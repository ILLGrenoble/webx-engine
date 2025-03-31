#ifndef WEBX_RECTANGLE_H
#define WEBX_RECTANGLE_H

#include <vector>
#include <algorithm>
#include <set>
#include <spdlog/spdlog.h>
#include "WebXSize.h"

/**
 * @class WebXRectangle
 * @brief Represents a rectangle with position, size, and utility methods for geometric operations.
 */
class WebXRectangle {
public:
    /**
     * @brief Default constructor for WebXRectangle.
     * Initializes the rectangle to (0, 0) with zero width and height.
     */
    WebXRectangle() :
        _x(0),
        _y(0),
        _left(0),
        _right(0),
        _top(0),
        _bottom(0) {}

    /**
     * @brief Constructs a WebXRectangle object with specified position and size.
     * @param x The x-coordinate of the bottom-left corner.
     * @param y The y-coordinate of the bottom-left corner.
     * @param width The width of the rectangle.
     * @param height The height of the rectangle.
     */
    WebXRectangle(int x, int y, int width, int height) :
        _x(x),
        _y(y),
        _size(WebXSize(width, height)),
        _left(x),
        _right(x + width),
        _top(y + height),
        _bottom(y) {}

    WebXRectangle(const WebXRectangle & rectangle) :
        _x(rectangle._x),
        _y(rectangle._y),
        _size(rectangle._size),
        _left(rectangle._left),
        _right(rectangle._right),
        _top(rectangle._top),
        _bottom(rectangle._bottom) {}
    virtual ~WebXRectangle() {}

    WebXRectangle & operator=(const WebXRectangle & rectangle) {
        if (this != &rectangle) {
            this->_x = rectangle._x;
            this->_y = rectangle._y;
            this->_size = rectangle._size;
            this->_left =rectangle._left;
            this->_right =rectangle._right;
            this->_top =rectangle._top;
            this->_bottom =rectangle._bottom;
        }
        return *this;
    }

    bool operator==(const WebXRectangle & rectangle) const {
        return this->_left == rectangle._left && this->_right == rectangle._right && this->_top == rectangle._top && this->_bottom == rectangle._bottom;
    }

    bool operator!=(const WebXRectangle & rectangle) const {
        return !operator==(rectangle);
    }

    /**
     * @brief Checks if the rectangle is visible within a given viewport.
     * @param viewport The viewport dimensions.
     * @return True if the rectangle is visible, false otherwise.
     */
    bool isVisible(const WebXSize & viewport) const {
        return
            this->_left < viewport.width() &&
            this->_right > 0 && 
            this->_bottom < viewport.height() &&
            this->_top > 0;
    }

    bool overlapOrTouching(const WebXRectangle & rectangle) const {
        return (
            this->_left < (rectangle._right + 1) &&
            this->_right > (rectangle._left - 1) &&
            this->_top > (rectangle._bottom - 1) &&
            this->_bottom < (rectangle._top + 1));
    }

    bool overlap(const WebXRectangle & rectangle) const {
        return (
            this->_left < rectangle._right &&
            this->_right > rectangle._left &&
            this->_top > rectangle._bottom &&
            this->_bottom < rectangle._top);
    }

    float overlapCoeff(const WebXRectangle & rectangle) const {
        int maxLeft = this->_left > rectangle._left ? this->_left : rectangle._left;
        int minRight = this->_right < rectangle._right ? this->_right : rectangle._right;
        int minTop = this->_top < rectangle._top ? this->_top : rectangle._top;
        int maxBottom = this->_bottom > rectangle._bottom ? this->_bottom : rectangle._bottom;

        int overlapWidth = minRight - maxLeft;
        int overlapHeight = minTop - maxBottom;

        if (overlapWidth <= 0 || overlapHeight <= 0) {
            return 0.0;
        }

        int intersectionArea = overlapWidth * overlapHeight;

        return (double)intersectionArea / this->area();
    }

    bool contains(const WebXRectangle & rectangle) const {
        return (
            this->_left <= rectangle._left &&
            this->_right >= rectangle._right &&
            this->_top >= rectangle._top &&
            this->_bottom <= rectangle._bottom);
    }

    int area() const {
        return this->_size.area();
    }

    WebXRectangle & operator+=(const WebXRectangle & rectangle) {

        int left = rectangle._left < this->_left ? rectangle._left : this->_left;
        int right = rectangle._right > this->_right ? rectangle._right : this->_right;
        int bottom = rectangle._bottom < this->_bottom ? rectangle._bottom : this->_bottom;
        int top = rectangle._top > this->_top ? rectangle._top : this->_top;

        this->_x = left;
        this->_y = bottom;
        this->_size = WebXSize(right - left, top - bottom);

        this->_left = left;
        this->_right = right;
        this->_bottom = bottom;
        this->_top = top;

        return *this;
    }

    int x() const {
        return this->_x;
    }

    int y() const {
        return this->_y;
    }

    const WebXSize & size() const {
        return this->_size;
    }

    int left() const {
        return this->_left;
    }

    int right() const {
        return this->_right;
    }

    int top() const {
        return this->_top;
    }

    int bottom() const {
        return this->_bottom;
    }
    
private:
    int _left;
    int _right;
    int _top;
    int _bottom;
    int _x;
    int _y;
    WebXSize _size;

};


#endif /* WEBX_RECTANGLE_H */