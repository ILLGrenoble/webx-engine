#ifndef WEBX_RECTANGLE_H
#define WEBX_RECTANGLE_H

#include <stdio.h>
#include "WebXSize.h"

class WebXRectangle {
public:
    WebXRectangle() :
        x(0),
        y(0),
        _left(0),
        _right(0),
        _top(0),
        _bottom(0) {}
    WebXRectangle(int x, int y, int width, int height) :
        x(x),
        y(y),
        size(WebXSize(width, height)),
        _left(x),
        _right(x + width),
        _top(y + height),
        _bottom(y) {}
    WebXRectangle(const WebXRectangle & rectangle) :
        x(rectangle.x),
        y(rectangle.y),
        size(rectangle.size),
        _left(rectangle._left),
        _right(rectangle._right),
        _top(rectangle._top),
        _bottom(rectangle._bottom) {}
    virtual ~WebXRectangle() {}

    WebXRectangle & operator=(const WebXRectangle & rectangle) {
        if (this != &rectangle) {
            this->x = rectangle.x;
            this->y = rectangle.y;
            this->size = rectangle.size;
            this->_left =rectangle._left;
            this->_right =rectangle._right;
            this->_top =rectangle._top;
            this->_bottom =rectangle._bottom;
        }
        return *this;
    }

    bool isVisible(const WebXSize & viewport) const {
        return
            this->_left < viewport.width &&
            this->_right > 0 && 
            this->_bottom < viewport.height &&
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

    bool contains(const WebXRectangle & rectangle) const {
        return (
            this->_left <= rectangle._left &&
            this->_right >= rectangle._right &&
            this->_top >= rectangle._top &&
            this->_bottom <= rectangle._bottom);
    }

    void clear() {
        this->x = 0;
        this->y = 0;
        this->size = WebXSize();
        this->_left = 0;
        this->_right = 0;
        this->_top = 0;
        this->_bottom = 0;
    }

    WebXRectangle & operator+=(const WebXRectangle & rectangle) {

        int left = rectangle._left < this->_left ? rectangle._left : this->_left;
        int right = rectangle._right > this->_right ? rectangle._right : this->_right;
        int bottom = rectangle._bottom < this->_bottom ? rectangle._bottom : this->_bottom;
        int top = rectangle._top > this->_top ? rectangle._top : this->_top;

        this->x = left;
        this->y = bottom;
        this->size.width = right - left;
        this->size.height = top - bottom;

        this->_left = left;
        this->_right = right;
        this->_bottom = bottom;
        this->_top = top;

        return *this;
    }

    int x;
    int y;
    WebXSize size;

private:
    int _left;
    int _right;
    int _top;
    int _bottom;
};


#endif /* WEBX_RECTANGLE_H */