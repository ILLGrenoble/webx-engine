#ifndef WEBX_KEYSYM_H
#define WEBX_KEYSYM_H

#include <string>

#define WEBX_MODIFIER_SHIFT_KEY 1
#define WEBX_MODIFIER_ALTGR_KEY 2

typedef struct WebXKeysym {
    std::string name;
    int keysym;
    std::string scancode;
    int keycode;
    int modifiers;

} WebXKeysym;

typedef struct WebXKeymap {
    std::string name;
    const WebXKeysym * mapping;
} WebXKeymap;

#endif /* WEBX_KEYSYM_H */
