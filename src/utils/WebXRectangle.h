#ifndef WEBX_RECTANGLE_H
#define WEBX_RECTANGLE_H

#include <stdio.h>

class WebXRectangle {
public:
    WebXRectangle() :
        x(0),
        y(0),
        width(0),
        height(0),
        _left(0),
        _right(0),
        _top(0),
        _bottom(0) {}
    WebXRectangle(int x, int y, int width, int height) :
        x(x),
        y(y),
        width(width),
        height(height),
        _left(x),
        _right(x + width),
        _top(y + height),
        _bottom(y) {}
    WebXRectangle(const WebXRectangle & rectangle) :
        x(rectangle.x),
        y(rectangle.y),
        width(rectangle.width),
        height(rectangle.height),
        _left(rectangle._left),
        _right(rectangle._right),
        _top(rectangle._top),
        _bottom(rectangle._bottom) {}
    virtual ~WebXRectangle() {}

    WebXRectangle & operator=(const WebXRectangle & rectangle) {
        if (this != &rectangle) {
            this->x = rectangle.x;
            this->y = rectangle.y;
            this->width = rectangle.width;
            this->height = rectangle.height;
            this->_left =rectangle._left;
            this->_right =rectangle._right;
            this->_top =rectangle._top;
            this->_bottom =rectangle._bottom;
        }
        return *this;
    }

    bool isVisible(const WebXRectangle & viewport) const {
        return
            this->_left < viewport.width &&
            this->_right > 0 && 
            this->_bottom < viewport.height &&
            this->_top > 0;
    }

    bool overlapOrTouhcing(const WebXRectangle & rectangle) const {
        return (
            this->_left < (rectangle._right + 1) &&
            this->_right > (rectangle._left - 1) &&
            this->_top > (rectangle._bottom - 1) &&
            this->_bottom < (rectangle._top + 1));
    }

    void clear() {
        this->x = 0;
        this->y = 0;
        this->width = 0;
        this->height = 0;
    }

    WebXRectangle & operator+=(const WebXRectangle & rectangle) {
        this->x = rectangle.x < this->x ? rectangle.x : this->x;
        this->y = rectangle.y < this->y ? rectangle.y : this->y;

        int maxX = (rectangle.x + rectangle.width) > (this->x + this->width) ? (rectangle.x + rectangle.width) : (this->x + this->width);
        int maxY = (rectangle.y + rectangle.height) > (this->y + this->height) ? (rectangle.y + rectangle.height) : (this->x + this->height);

        this->width = maxX - this->x;
        this->height = maxY - this->y;

        this->_left = x;
        this->_right = x + width;
        this->_bottom = y;
        this->_top = y + height;

        return *this;
    }


    int x;
    int y;
    int width;
    int height;

private:
    int _left;
    int _right;
    int _top;
    int _bottom;
};


#endif /* WEBX_RECTANGLE_H */