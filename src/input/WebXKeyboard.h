#ifndef WEBX_KEYBOARD_H
#define WEBX_KEYBOARD_H

#include <X11/Xlib.h>

class WebXKeyboard {

public:
    WebXKeyboard(Display * x11Display);

    ~WebXKeyboard();

    void press(int key);

    void release(int key);

private:
    Display * _x11Display;
    int getMappedKey(int key) const;

};


#endif //WEBX_KEYBOARD_H
