#ifndef WEBX_KEYBOARD_H
#define WEBX_KEYBOARD_H

#include "WebXKeyboardMapping.h"
#include <X11/Xlib.h>

#define WEBX_XBD_RULES_PATH "/usr/share/X11/xkb/rules/"

class WebXKeyboard {

public:
    WebXKeyboard(Display * x11Display);
    ~WebXKeyboard();

    void init();

    void handleKeySym(int keysym, bool pressed, bool isFromClient = false) const;

private:
    int getMappedKey(int key) const;
    const WebXKeySymDefinition * getKeySymDefinition(int keysym) const;
    void clearLocks() const;
    void updateModifiers(int modifiersToSet, int modifiersToClear) const;
    void releaseModifiers(int modifiers) const;

private:
    Display * _x11Display;
    WebXKeyboardMapping * _keyboardMapping;
};


#endif //WEBX_KEYBOARD_H
