#ifndef WEBX_KB_H
#define WEBX_KB_H

#include <string>
#include <X11/Xlib.h>

#define WEBX_XBD_RULES_PATH "/usr/share/X11/xkb/rules/"

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

class WebXKB {

public:
    WebXKB() {}
    ~WebXKB() {}

    std::string getKeyboardLayout(Display * display) const;
    void setKeyboardLayout(Display * display, const std::string & layout) const;

private:
    bool initialiseKeyboardSettings(Display * display, WebXKBSettings & settings) const;
    bool sendKeyboardSettings(Display * display, WebXKBSettings & settings) const;
    bool applyRules(WebXKBSettings & settings) const;

};


#endif //WEBX_KB_H
