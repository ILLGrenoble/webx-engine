#ifndef WEBX_KEYSYM_H
#define WEBX_KEYSYM_H

#include <string>
#include <map>

/**
 * The X11 keysym for Num Lock.
 */
#define WEBX_KEYBOARD_KEYSYM_NUM_LOCK 0xFF7F

/**
 * The X11 keysym for Scroll Lock.
 */
#define WEBX_KEYBOARD_KEYSYM_SCROLL_LOCK 0xFF14

/**
 * The X11 keysym for Caps Lock.
 */
#define WEBX_KEYBOARD_KEYSYM_CAPS_LOCK 0xFFE5

/**
 * The X11 keysym for Left Shift.
 */
#define WEBX_KEYBOARD_KEYSYM_LSHIFT 0xFFE1

/**
 * The X11 keysym for Right Shift.
 */
#define WEBX_KEYBOARD_KEYSYM_RSHIFT 0xFFE2

/**
 * The X11 keysym for Left Ctrl.
 */
#define WEBX_KEYBOARD_KEYSYM_LCTRL 0xFFE3

/**
 * The X11 keysym for Right Ctrl.
 */
#define WEBX_KEYBOARD_KEYSYM_RCTRL 0xFFE4

/**
 * The X11 keysym for Left Alt.
 */
#define WEBX_KEYBOARD_KEYSYM_LALT 0xFFE9

/**
 * The X11 keysym for Right Alt.
 */
#define WEBX_KEYBOARD_KEYSYM_RALT 0xFFEA

/**
 * The X11 keysym for AltGr.
 */
#define WEBX_KEYBOARD_KEYSYM_ALTGR 0xFE03

/**
 * Modifier key for Shift.
 */
#define WEBX_SHIFT_MODIFIER_KEY 1

/**
 * Modifier key for AltGr.
 */
#define WEBX_ALTGR_MODIFIER_KEY 2

/**
 * Lock key for Num Lock.
 */
#define WEBX_NUM_LOCK_KEY 1

/**
 * Represents a keysym definition, including its name, keysym, keycode, and modifier/lock settings.
 */
typedef struct WebXKeySymDefinition {
    std::string name;
    int keysym;
    int keycode;
    int modifiersToSet;
    int modifiersToClear;
    int locksToSet;

} WebXKeySymDefinition;

/**
 * Represents a keyboard mapping, including its name, layout, and key mapping.
 */
typedef struct WebXKeyboardMapping {
    std::string name;
    std::string layout;
    const std::map<int, WebXKeySymDefinition> & mapping;
} WebXKeyboardMapping;

#endif /* WEBX_KEYSYM_H */
