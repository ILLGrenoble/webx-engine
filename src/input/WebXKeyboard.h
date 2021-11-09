#ifndef WEBX_KEYBOARD_H
#define WEBX_KEYBOARD_H

#include "WebXKeyboardMapping.h"
#include <X11/Xlib.h>

class WebXKeyboard {

public:
    WebXKeyboard(Display * x11Display);
    ~WebXKeyboard();

    void init();

    void handleKeySym(int keysym, bool pressed, bool isFromClient = false) const;

    // void press(int key);

    // void release(int key);

private:
    // int getMappedKey(int key) const;
    // KeyCode getMappedKeyAsKeyCode(int mappedKey) const;
    const WebXKeySymDefinition * getKeySymDefinition(int keysym) const;
    void clearModifiers() const;
    void setModifiers(int modifiers) const;
    void releaseModifiers(int modifiers) const;

private:
    Display * _x11Display;
    WebXKeyboardMapping * _keyboardMapping;
};


#endif //WEBX_KEYBOARD_H
