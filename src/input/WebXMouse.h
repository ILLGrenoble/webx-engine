#include <connector/instruction/WebXMouseInstruction.h>
#include "WebXMouseState.h"
#include "display/WebXWindow.h"

#ifndef WEBX_MOUSE_H
#define WEBX_MOUSE_H

#define LeftButtonMask Button1Mask
#define MiddleButtonMask Button2Mask
#define RightButtonMask Button3Mask
#define ScrollUpButtonMask Button4Mask
#define ScrollDownButtonMask Button5Mask
#define LeftButton Button1
#define MiddleButton Button2
#define RightButton Button3
#define ScrollUpButton Button4
#define ScrollDownButton Button5

class WebXMouse {

public:
    WebXMouse(Display * x11Display, const Window &rootWindow) {
        _x11Display = x11Display;
        _rootWindow = rootWindow;
    }

    ~WebXMouse() {
    }

    void updateMouse(int x, int y, unsigned int buttonMask);

private:
    WebXMouseState _currentMouseState = WebXMouseState();
    Display * _x11Display;
    Window  _rootWindow;
    void sendMouseButtons(unsigned int newButtonMask);
    void sendMouseButton(unsigned int button, Bool isPressed);
    void sendMouseMovement(int newX, int newY);
    void updateMouseState(int newX, int newY, int newButtonMask);
};

#endif //WEBX_MOUSE_H
