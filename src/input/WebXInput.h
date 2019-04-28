#ifndef WEBX_INPUT_H
#define WEBX_INPUT_H

#include "input/WebXMouse.h"
/**
 * Represents the input class for managing keyboard and mouse events
 */
class WebXInput {
public:
    WebXInput(WebXMouse * mouse);

    ~WebXInput();

    WebXMouse * getMouse() const {
        return _mouse;
    }

private:
    WebXMouse * _mouse;
};


#endif //WEBX_INPUT_H
