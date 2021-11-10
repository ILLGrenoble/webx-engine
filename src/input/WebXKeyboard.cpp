#include "WebXKeyboard.h"
#include "WebXKB.h"
#include "WebXKeyboardSymbolMapping.h"
#include <X11/extensions/XTest.h>
#include <spdlog/spdlog.h>
#include <algorithm>
#include <iterator>

WebXKeyboard::WebXKeyboard(Display * x11Display) :
    _x11Display(x11Display),
    _keyboardMapping(NULL) {
}

WebXKeyboard::~WebXKeyboard() {
}

void WebXKeyboard::init() {
    WebXKB webXKB;
    std::string layout = webXKB.getKeyboardLayout(this->_x11Display);
    spdlog::info("Got keyboard layout '{:s}'", layout);
    webXKB.setKeyboardLayout(this->_x11Display, "fr");

    // std::string mappingName = "en_gb_qwerty";
    // std::string mappingName = "en_us_qwerty";
    std::string mappingName = "fr_fr_azerty";

    // Dummy init of keyboard. Using French keyboard symbol mapping
    auto it = std::find_if(begin(WEBX_KEY_MAPS), end(WEBX_KEY_MAPS), [=](const WebXKeyboardMapping & mapping) {
        return mapping.name == mappingName;
    });

    if (it != WEBX_KEY_MAPS.end()) {
        WebXKeyboardMapping & keyboardMapping = *it;
        this->_keyboardMapping = &(keyboardMapping);

        spdlog::info("Loading keyboard mapping '{:s}'", keyboardMapping.name);
    
    } else {
        spdlog::error("Could not find keyboard mapping for '{:s}'", mappingName);
    }
}

int WebXKeyboard::getMappedKey(int key) const {
    /* Map key between 0x0000 and 0xFFFF directly */
    if (key >= 0x0000 && key <= 0xFFFF) {
        return key;
    }

    /* Map all Unicode keys from U+0000 to U+FFFF */
    if (key >= 0x1000000 && key <= 0x100FFFF) {
        return 0x10000 + (key & 0xFFFF);
    }

    /* All other keys are unmapped */
    return 0;
}


// KeyCode WebXKeyboard::getMappedKeyAsKeyCode(int mappedKey) const {
//     return XKeysymToKeycode(this->_x11Display, mappedKey);
// }

void WebXKeyboard::handleKeySym(int keysym, bool pressed, bool isFromClient) const {
    if (this->_keyboardMapping == NULL) {
        return;
    }

    // int mappedKey = this->getMappedKey(keysym);
    int mappedKey = keysym;

    // Convert keysym to KeySymDefinition
    const WebXKeySymDefinition * keysymDef = this->getKeySymDefinition(mappedKey);
    if (keysymDef != NULL) {
        if (pressed) {
            // Clear all modifier if from client
            if (isFromClient) {
                this->clearLocks();
            }

            // Set all necessary modifiers
            this->updateModifiers(keysymDef->modifiersToSet, keysymDef->modifiersToClear);

            // Press keysym->keycode mapping
            spdlog::trace("Press for keycode '{:d}' for kesym {:d} ('{:s}')", keysymDef->keycode, keysymDef->keysym, keysymDef->name);
            XTestFakeKeyEvent(this->_x11Display, keysymDef->keycode, True, 0);

        } else {
            // Release keysym->keycode mapping
            spdlog::trace("Release for keycode '{:d}' for keysym {:d} ('{:s}')", keysymDef->keycode, keysymDef->keysym, keysymDef->name);
            XTestFakeKeyEvent(this->_x11Display, keysymDef->keycode, False, 0);

            // Release all used modifiers to clean up the state
            this->releaseModifiers(keysymDef->modifiersToSet);
        }

        XFlush(this->_x11Display);

    } else {
        spdlog::trace("[Warn] Could not find keyboard mapping for keysym '{:d}' for {:s}", keysym, pressed ? "press" : "release");
    }
}

const WebXKeySymDefinition * WebXKeyboard::getKeySymDefinition(int keysym) const  {
    const std::map<int, WebXKeySymDefinition> & mapping = this->_keyboardMapping->mapping;
    auto keysymDefIt = mapping.find(keysym);
    if (keysymDefIt != mapping.end()) {
        const WebXKeySymDefinition & keysymDef = keysymDefIt->second;

        return &keysymDef;
    }
    return NULL;
}

void WebXKeyboard::clearLocks() const {
    this->handleKeySym(WEBX_KEYBOARD_KEYSYM_NUM_LOCK, false);
    this->handleKeySym(WEBX_KEYBOARD_KEYSYM_SCROLL_LOCK, false);
    this->handleKeySym(WEBX_KEYBOARD_KEYSYM_CAPS_LOCK, false);
}

void WebXKeyboard::updateModifiers(int modifiersToSet, int modifiersToClear) const {
    if (modifiersToSet & WEBX_SHIFT_MODIFIER_KEY) {
        this->handleKeySym(WEBX_KEYBOARD_KEYSYM_LSHIFT, true);
    
    } else if (modifiersToClear & WEBX_SHIFT_MODIFIER_KEY) {
        this->handleKeySym(WEBX_KEYBOARD_KEYSYM_LSHIFT, false);
        this->handleKeySym(WEBX_KEYBOARD_KEYSYM_RSHIFT, false);
    }

    if (modifiersToSet & WEBX_ALTGR_MODIFIER_KEY) {
        this->handleKeySym(WEBX_KEYBOARD_KEYSYM_ALTGR, true);
    
    } else if (modifiersToClear & WEBX_ALTGR_MODIFIER_KEY) {
        this->handleKeySym(WEBX_KEYBOARD_KEYSYM_ALTGR, false);
        this->handleKeySym(WEBX_KEYBOARD_KEYSYM_LALT, false);
        this->handleKeySym(WEBX_KEYBOARD_KEYSYM_RALT, false);
    }
}

void WebXKeyboard::releaseModifiers(int modifiers) const {
    if (modifiers & WEBX_SHIFT_MODIFIER_KEY) {
        this->handleKeySym(WEBX_KEYBOARD_KEYSYM_LSHIFT, false);
    }

    if (modifiers & WEBX_ALTGR_MODIFIER_KEY) {
        this->handleKeySym(WEBX_KEYBOARD_KEYSYM_ALTGR, false);
    }
}
