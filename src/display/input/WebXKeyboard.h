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

    void handleKeySym(int keysym, bool pressed, bool isFromClient = false);
    bool loadKeyboardLayout(const std::string & layout);

private:
    WebXKeyboardMapping * getKeyboardMapping(const std::string & layout) const;
    int getMappedKey(int key) const;
    const WebXKeySymDefinition * getKeySymDefinition(int keysym) const;
    bool ignoreKeysym(int keysym) const;
    
    void updateModifiers(int modifiersToSet, int modifiersToClear);
    void releaseModifiers(int modifiers);

    void clearLocks();
    void updateLocks(int locks);
    void releaseLocks(int locks);

private:
    Display * _x11Display;
    WebXKeyboardMapping * _keyboardMapping;
    static std::string DEFAULT_KEYBOARD_LAYOUT;
    int _locksSet;
};


#endif //WEBX_KEYBOARD_H
