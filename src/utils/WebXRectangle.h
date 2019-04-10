#ifndef WEBX_RECTANGLE_H
#define WEBX_RECTANGLE_H

#include <stdio.h>

class WebXRectangle {
public:
    WebXRectangle() :
        x(0),
        y(0),
        width(0),
        height(0) {}
    WebXRectangle(int x, int y, int width, int height) :
        x(x),
        y(y),
        width(width),
        height(height) {}
    WebXRectangle(const WebXRectangle & rectangle) :
        x(rectangle.x),
        y(rectangle.y),
        width(rectangle.width),
        height(rectangle.height) {}
    virtual ~WebXRectangle() {}

    WebXRectangle & operator=(const WebXRectangle & rectangle) {
        if (this != &rectangle) {
            this->x = rectangle.x;
            this->y = rectangle.y;
            this->width = rectangle.width;
            this->height = rectangle.height;
        }
        return *this;
    }

    bool isVisible(const WebXRectangle & viewport) const {
        return
            this->x < viewport.width &&
            this->x + this->width > 0 && 
            this->y < viewport.height &&
            this->y + this->height > 0;
    } 

    void clear() {
        this->x = 0;
        this->y = 0;
        this->width = 0;
        this->height = 0;
    }

    void combine(const WebXRectangle & rectangle) {
        this->x = rectangle.x < this->x ? rectangle.x : this->x;
        this->y = rectangle.y < this->y ? rectangle.y : this->y;

        int maxX = (rectangle.x + rectangle.width) > (this->x + this->width) ? (rectangle.x + rectangle.width) : (this->x + this->width);
        int maxY = (rectangle.y + rectangle.height) > (this->y + this->height) ? (rectangle.y + rectangle.height) : (this->x + this->height);

        this->width = maxX - this->x;
        this->height = maxY - this->y;
    }

    int x;
    int y;
    int width;
    int height;
};


#endif /* WEBX_RECTANGLE_H */