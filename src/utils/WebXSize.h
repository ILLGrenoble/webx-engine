#ifndef WEBX_SIZE_H
#define WEBX_SIZE_H

class WebXSize {
public:
    WebXSize() :
        width(0),
        height(0) {}
    WebXSize(unsigned int width, unsigned int height) :
        width(width),
        height(height) {}
    virtual ~WebXSize() {}

    unsigned int width;
    unsigned int height;
};


#endif /* WEBX_SIZE_H */