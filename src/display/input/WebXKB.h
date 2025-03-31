#ifndef WEBX_KB_H
#define WEBX_KB_H

#include <string>
#include <X11/Xlib.h>

#define WEBX_XBD_RULES_PATH "/usr/share/X11/xkb/rules/"

/**
 * Represents the settings for the XKB (X Keyboard Extension).
 */
typedef struct WebXKBSettings {
    std::string rules;
    std::string model;
    std::string layout;
    std::string variant;
    std::string keycodes;
    std::string types;
    std::string compat;
    std::string symbols;
    std::string geometry;
    std::string keymap;
} WebXKBSettings;

/**
 * Represents the WebXKB, which manages keyboard layouts and settings using XKB.
 */
class WebXKB {

public:
    /**
     * Constructor for WebXKB.
     */
    WebXKB() {}

    /**
     * Destructor for WebXKB.
     */
    ~WebXKB() {}

    /**
     * Get the current keyboard layout.
     * @param display The X11 display connection.
     * @return The name of the current keyboard layout.
     */
    std::string getKeyboardLayout(Display * display) const;

    /**
     * Set a new keyboard layout.
     * @param display The X11 display connection.
     * @param layout The name of the layout to set (e.g., "us", "de").
     * @return True if the layout was successfully set, false otherwise.
     */
    bool setKeyboardLayout(Display * display, const std::string & layout) const;

private:
    /**
     * Initialize the keyboard settings.
     * @param display The X11 display connection.
     * @param settings The WebXKBSettings object to populate.
     * @return True if the settings were successfully initialized, false otherwise.
     */
    bool initialiseKeyboardSettings(Display * display, WebXKBSettings & settings) const;

    /**
     * Send the keyboard settings to the X server.
     * @param display The X11 display connection.
     * @param settings The WebXKBSettings object to send.
     * @return True if the settings were successfully sent, false otherwise.
     */
    bool sendKeyboardSettings(Display * display, WebXKBSettings & settings) const;

    /**
     * Apply the XKB rules to the settings.
     * @param settings The WebXKBSettings object to modify.
     * @return True if the rules were successfully applied, false otherwise.
     */
    bool applyRules(WebXKBSettings & settings) const;

};

#endif //WEBX_KB_H
