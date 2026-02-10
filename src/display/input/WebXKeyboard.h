#ifndef WEBX_KEYBOARD_H
#define WEBX_KEYBOARD_H

#include "WebXKeyboardMapping.h"
#include <X11/Xlib.h>

#define WEBX_XBD_RULES_PATH "/usr/share/X11/xkb/rules/"

/**
 * Represents the WebXKeyboard, which manages keyboard layouts and handle keyboard events from clients.
 */
class WebXKeyboard {

public:
    /**
     * Constructor for WebXKeyboard.
     * @param x11Display The X11 display connection.
     */
    WebXKeyboard(Display * x11Display);

    /**
     * Destructor for WebXKeyboard.
     */
    ~WebXKeyboard();

    /**
     * Initialize the keyboard settings.
     */
    void init();

    /**
     * Handle a keysym event.
     * @param keysym The keysym to handle.
     * @param pressed Whether the key is pressed or released.
     * @param isFromClient Whether the event is from a client (default is false).
     */
    void handleKeySym(int keysym, bool pressed, bool isFromClient = false);

    /**
     * Load a keyboard layout.
     * @param layout The name or the layout to load (e.g., "us", "de" or "fr_fr_azerty", etc).
     * @return True if the layout was successfully loaded, false otherwise.
     */
    bool loadKeyboardLayout(const std::string & layoutOrName);

    /**
     * @brief Returns the current keyboard layout name.
     * @return the current keyboard layout name.
     */
    std::string getKeyboardLayoutName() const {
        if (this->_keyboardMapping != nullptr) {
            return this->_keyboardMapping->name;
        }
        return "";
    }

private:
    /**
     * Get the keyboard mapping for a specific layout.
     * @param layout The name of the layout.
     * @return A pointer to the WebXKeyboardMapping object.
     */
    WebXKeyboardMapping * getKeyboardMapping(const std::string & layout) const;

    /**
     * Get the mapped key for a given keycode.
     * @param key The keycode to map.
     * @return The mapped key.
     */
    int getMappedKey(int key) const;

    /**
     * Get the keysym definition for a given keysym.
     * @param keysym The keysym to look up.
     * @return A pointer to the WebXKeySymDefinition object.
     */
    const WebXKeySymDefinition * getKeySymDefinition(int keysym) const;

    /**
     * Determine whether a keysym should be ignored.
     * @param keysym The keysym to check.
     * @return True if the keysym should be ignored, false otherwise.
     */
    bool ignoreKeysym(int keysym) const;

    /**
     * Update the modifier keys.
     * @param modifiersToSet Modifiers to set.
     * @param modifiersToClear Modifiers to clear.
     */
    void updateModifiers(int modifiersToSet, int modifiersToClear);

    /**
     * Release specific modifier keys.
     * @param modifiers The modifiers to release.
     */
    void releaseModifiers(int modifiers);

    /**
     * Clear all lock keys.
     */
    void clearLocks();

    /**
     * Update the lock keys.
     * @param locks The locks to set.
     */
    void updateLocks(int locks);

    /**
     * Release specific lock keys.
     * @param locks The locks to release.
     */
    void releaseLocks(int locks);

private:
    Display * _x11Display;
    WebXKeyboardMapping * _keyboardMapping;
    static std::string DEFAULT_KEYBOARD_LAYOUT;
    int _locksSet;
};

#endif //WEBX_KEYBOARD_H
