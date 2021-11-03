#ifndef WEBX_KEYSYM_H
#define WEBX_KEYSYM_H

#include <string>

#define WEBX_MODIFIER_SHIFT_KEY 1
#define WEBX_MODIFIER_ALTGR_KEY 2

typedef struct WebXKeySymDefinition {
    std::string name;
    int keysym;
    std::string scancode;
    int keycode;
    int modifiers;

} WebXKeySymDefinition;

typedef struct WebXKeyboardMapping {
    std::string name;
    const WebXKeySymDefinition * mapping;
} WebXKeyboardMapping;

#endif /* WEBX_KEYSYM_H */
