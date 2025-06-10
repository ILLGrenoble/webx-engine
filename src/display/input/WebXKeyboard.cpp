#include "WebXKeyboard.h"
#include "WebXKB.h"
#include "WebXKeyboardSymbolMapping.h"
#include <X11/extensions/XTest.h>
#include <spdlog/spdlog.h>
#include <algorithm>
#include <iterator>

std::string WebXKeyboard::DEFAULT_KEYBOARD_LAYOUT = "gb";

WebXKeyboard::WebXKeyboard(Display * x11Display) :
    _x11Display(x11Display),
    _keyboardMapping(NULL),
    _locksSet(0) {
}

WebXKeyboard::~WebXKeyboard() {
}

void WebXKeyboard::init() {
    WebXKB webXKB;
    std::string layout = webXKB.getKeyboardLayout(this->_x11Display);
    spdlog::info("Got keyboard layout '{:s}' from X11. Searching for mapping...", layout);
    WebXKeyboardMapping * keyboardMapping = this->getKeyboardMapping(layout);
    if (keyboardMapping != NULL) {
        spdlog::info("... loaded keyboard mapping '{:s}'", keyboardMapping->name);
        this->_keyboardMapping = keyboardMapping;

        // Make sure the keyboard layout really is loaded (reload the current one. Fixes a bug where XKB behaved as if US was loaded)
        this->loadKeyboardLayout(layout);

    } else {
        if (this->loadKeyboardLayout(DEFAULT_KEYBOARD_LAYOUT)) {
            spdlog::warn("... could not find keyboard mapping for '{:s}'. Loading default {:s} layout and sent to X11.", keyboardMapping->name, DEFAULT_KEYBOARD_LAYOUT);

        } else {
            spdlog::error("... could not find keyboard mapping for '{:s}' and failed to send default {:s} layout to X11.", keyboardMapping->name, DEFAULT_KEYBOARD_LAYOUT);
        }
    }
}

WebXKeyboardMapping * WebXKeyboard::getKeyboardMapping(const std::string & layout) const {
    auto it = std::find_if(begin(WEBX_KEY_MAPS), end(WEBX_KEY_MAPS), [=](const WebXKeyboardMapping & mapping) {
        return mapping.layout == layout || mapping.name == layout;
    });

    if (it != WEBX_KEY_MAPS.end()) {
        WebXKeyboardMapping & keyboardMapping = *it;
        return &keyboardMapping;
    
    } else {
        return NULL;
    }
}

bool WebXKeyboard::loadKeyboardLayout(const std::string & layoutOrName) {
    WebXKB webXKB;
    WebXKeyboardMapping * keyboardMapping = this->getKeyboardMapping(layoutOrName);
    if (keyboardMapping != NULL) {
        this->_keyboardMapping = keyboardMapping;

        return webXKB.setKeyboardLayout(this->_x11Display, keyboardMapping->layout);
    }
    return false;
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

void WebXKeyboard::handleKeySym(int keysym, bool pressed, bool isFromClient) {
    if (this->_keyboardMapping == NULL) {
        return;
    }

    // int mappedKey = this->getMappedKey(keysym);
    int mappedKey = keysym;

    // Convert keysym to KeySymDefinition
    const WebXKeySymDefinition * keysymDef = this->getKeySymDefinition(mappedKey);
    if (keysymDef != NULL && !ignoreKeysym(keysym)) {
        if (pressed) {
            // Set all necessary modifiers
            this->updateModifiers(keysymDef->modifiersToSet, keysymDef->modifiersToClear);

            // Set all necessary locks
            this->updateLocks(keysymDef->locksToSet);

            // Press keysym->keycode mapping
            spdlog::trace("Press for keycode '{:d}' for kesym {:d} ('{:s}')", keysymDef->keycode, keysymDef->keysym, keysymDef->name);
            XTestFakeKeyEvent(this->_x11Display, keysymDef->keycode, True, 0);

        } else {
            // Release keysym->keycode mapping
            spdlog::trace("Release for keycode '{:d}' for keysym {:d} ('{:s}')", keysymDef->keycode, keysymDef->keysym, keysymDef->name);
            XTestFakeKeyEvent(this->_x11Display, keysymDef->keycode, False, 0);

            // Release all used modifiers to clean up the state
            this->releaseModifiers(keysymDef->modifiersToSet);

            if (isFromClient) {
                this->releaseLocks(keysymDef->locksToSet);
            }
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

bool WebXKeyboard::ignoreKeysym(int keysym) const {
    return keysym == WEBX_KEYBOARD_KEYSYM_CAPS_LOCK;
}

void WebXKeyboard::updateModifiers(int modifiersToSet, int modifiersToClear) {
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

void WebXKeyboard::releaseModifiers(int modifiers) {
    if (modifiers & WEBX_SHIFT_MODIFIER_KEY) {
        this->handleKeySym(WEBX_KEYBOARD_KEYSYM_LSHIFT, false);
    }

    if (modifiers & WEBX_ALTGR_MODIFIER_KEY) {
        this->handleKeySym(WEBX_KEYBOARD_KEYSYM_ALTGR, false);
    }
}

void WebXKeyboard::clearLocks() {

    if (this->_locksSet & WEBX_NUM_LOCK_KEY) {
        this->handleKeySym(WEBX_KEYBOARD_KEYSYM_NUM_LOCK, true);
        this->handleKeySym(WEBX_KEYBOARD_KEYSYM_NUM_LOCK, false);
    }
    this->_locksSet = 0;
}

void WebXKeyboard::updateLocks(int locks) {
    if (locks & WEBX_NUM_LOCK_KEY && (this->_locksSet & WEBX_NUM_LOCK_KEY) == 0) {
        this->handleKeySym(WEBX_KEYBOARD_KEYSYM_NUM_LOCK, true);
        this->handleKeySym(WEBX_KEYBOARD_KEYSYM_NUM_LOCK, false);

        // Set numlock bit
        this->_locksSet |= WEBX_NUM_LOCK_KEY;
    }
}

void WebXKeyboard::releaseLocks(int locks) {
    if (locks & WEBX_NUM_LOCK_KEY && (this->_locksSet & WEBX_NUM_LOCK_KEY) != 0) {
        this->handleKeySym(WEBX_KEYBOARD_KEYSYM_NUM_LOCK, true);
        this->handleKeySym(WEBX_KEYBOARD_KEYSYM_NUM_LOCK, false);
        
        // Clear numlock bit
        this->_locksSet &= ~WEBX_NUM_LOCK_KEY;
    }
}
