#ifndef WEBX_KEYBOARD_H
#define WEBX_KEYBOARD_H

#include "WebXKeyboardMapping.h"
#include <X11/Xlib.h>

class WebXKeyboard {

public:
    WebXKeyboard(Display * x11Display);
    ~WebXKeyboard();

    void init();

    void handleKeySym(int keysym, bool pressed);

    void press(int key);

    void release(int key);

private:
    int getMappedKey(int key) const;
    KeyCode getMappedKeyAsKeyCode(int mappedKey) const;

private:
    Display * _x11Display;
    WebXKeyboardMapping * _keyboardMapping;
};


#endif //WEBX_KEYBOARD_H
