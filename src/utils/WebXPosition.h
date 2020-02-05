#ifndef WEBX_POSITON_H
#define WEBX_POSITON_H

class WebXPosition {
public:
    WebXPosition() :
        x(0),
        y(0) {}
    WebXPosition(int x, int y) :
        x(x),
        y(y) {}
    virtual ~WebXPosition() {}

    bool operator==(const WebXPosition & position) {
        return (this == &position || (this->x == position.x && this->y == position.y));
    }

    bool operator!=(const WebXPosition & position) {
        return !this->operator==(position);
    }

    int x;
    int y;
};


#endif /* WEBX_POSITON_H */