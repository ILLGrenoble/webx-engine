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

#include "WebXMouseState.h"
#include "cursor/WebXMouseCursorFactory.h"

class WebXMouse {
public:
    WebXMouse(Display * x11Display, Window rootWindow);
    virtual ~WebXMouse();

    void sendClientInstruction(int x, int y, unsigned int buttonMask);
    
    WebXMouseState * getState() {
        return this->_state;
    }

    void updateCursor();
    std::shared_ptr<WebXMouseCursor> getCursor(uint32_t cursorId = 0);
    void updatePosition(int x, int y);
    void updatePosition();

private:
    WebXMouseState * createDefaultMouseState();
    void sendMouseButtons(unsigned int newButtonMask);
    void sendMouseButton(unsigned int button, Bool isPressed);
    void sendMouseMovement(int newX, int newY);

private:
    Display * _x11Display;
    Window  _rootWindow;
    WebXMouseCursorFactory _cursorFactory;
    WebXMouseState  * _state;
};

#endif //WEBX_MOUSE_H
