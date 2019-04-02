#ifndef WEBX_RECTANGLE_H
#define WEBX_RECTANGLE_H

class WebXRectangle {
public:
    WebXRectangle() :
        x(0),
        y(0),
        width(0),
        height(0) {}
    WebXRectangle(unsigned int x, unsigned int y, unsigned int width, unsigned int height) :
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
            this->x + width > 0 && 
            this->y < viewport.height &&
            this->y + height > 0;
    } 

    unsigned int x;
    unsigned int y;
    unsigned int width;
    unsigned int height;
};


#endif /* WEBX_RECTANGLE_H */