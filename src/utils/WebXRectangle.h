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
    virtual ~WebXRectangle() {}

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