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

#include <X11/extensions/Xfixes.h>
#include "input/cursor/WebXMouseCursor.h"

class WebXMouseState;

class WebXMouse {
public:
    WebXMouse(Display * x11Display, const Window &rootWindow);
    ~WebXMouse();

    void updateMouse(int x, int y, unsigned int buttonMask);
    WebXMouseState  * createDefaultMouseState();

private:
    void sendMouseButtons(unsigned int newButtonMask);
    void sendMouseButton(unsigned int button, Bool isPressed);
    void sendMouseMovement(int newX, int newY);
    void sendCursor();
    WebXMouseCursor * getCursor();
    void updateMouseState(int newX, int newY, int newButtonMask);

private:
    Display * _x11Display;
    Window  _rootWindow;
    WebXMouseState  * _currentMouseState;
};

#endif //WEBX_MOUSE_H
