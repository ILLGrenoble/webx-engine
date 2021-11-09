#include "WebXKeyboard.h"
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
    std::string mappingName = "en_us_qwerty";
    // std::string mappingName = "fr_fr_azerty";

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

// int WebXKeyboard::getMappedKey(int key) const {
//     /* Map key between 0x0000 and 0xFFFF directly */
//     if (key >= 0x0000 && key <= 0xFFFF) {
//         return key;
//     }

//     /* Map all Unicode keys from U+0000 to U+FFFF */
//     if (key >= 0x1000000 && key <= 0x100FFFF) {
//         return 0x10000 + (key & 0xFFFF);
//     }

//     /* All other keys are unmapped */
//     return 0;
// }


// KeyCode WebXKeyboard::getMappedKeyAsKeyCode(int mappedKey) const {
//     return XKeysymToKeycode(this->_x11Display, mappedKey);
// }

void WebXKeyboard::handleKeySym(int keysym, bool pressed, bool isFromClient) const {
    if (this->_keyboardMapping == NULL) {
        return;
    }

    // int mappedKey = this->getMappedKey(keysym);

    // Convert keysym to KeySymDefinition
    const WebXKeySymDefinition * keysymDef = this->getKeySymDefinition(keysym);
    if (keysymDef != NULL) {
        if (pressed) {

            // Clear all modifier if from client
            if (isFromClient) {
                this->clearModifiers();
            }

            // Set all necessary modifiers
            this->setModifiers(keysymDef->modifiers);

            // Press keysym->keycode mapping
            XTestFakeKeyEvent(this->_x11Display, keysymDef->keycode, True, 0);

        } else {
            // Release keysym->keycode mapping
            XTestFakeKeyEvent(this->_x11Display, keysymDef->keycode, False, 0);

            // Set all necessary modifiers
            this->releaseModifiers(keysymDef->modifiers);
        }

    } else {
        spdlog::warn("Could not find keyboard mapping for keysym '{:d}'", keysym);
    }

}

const WebXKeySymDefinition * WebXKeyboard::getKeySymDefinition(int keysym) const  {
    const std::map<int, WebXKeySymDefinition> & mapping = this->_keyboardMapping->mapping;
    auto keysymDefIt = mapping.find(keysym);
    if (keysymDefIt != mapping.end()) {
        const WebXKeySymDefinition & keysymDef = keysymDefIt->second;

        return &keysymDef;
    }
    spdlog::warn("Could not find keyboard mapping for keysym '{:d}'", keysym);
    return NULL;
}

void WebXKeyboard::clearModifiers() const {
    // Clear SHIFT and ALTGR
    this->handleKeySym(WEBX_KEYBOARD_KEYSYM_LSHIFT, false);
    this->handleKeySym(WEBX_KEYBOARD_KEYSYM_RSHIFT, false);
    this->handleKeySym(WEBX_KEYBOARD_KEYSYM_LCTRL, false);
    this->handleKeySym(WEBX_KEYBOARD_KEYSYM_RCTRL, false);
    this->handleKeySym(WEBX_KEYBOARD_KEYSYM_LALT, false);
    this->handleKeySym(WEBX_KEYBOARD_KEYSYM_RALT, false);
    this->handleKeySym(WEBX_KEYBOARD_KEYSYM_ALTGR, false);
    
    this->handleKeySym(WEBX_KEYBOARD_KEYSYM_NUM_LOCK, false);
    this->handleKeySym(WEBX_KEYBOARD_KEYSYM_SCROLL_LOCK, false);
    this->handleKeySym(WEBX_KEYBOARD_KEYSYM_CAPS_LOCK, false);
    this->handleKeySym(WEBX_KEYBOARD_KEYSYM_KANA_LOCK, false);
}

void WebXKeyboard::setModifiers(int modifiers) const {
    if (modifiers & WEBX_SHIFT_MODIFIER_KEY) {
        this->handleKeySym(WEBX_KEYBOARD_KEYSYM_LSHIFT, true);
    }
    if (modifiers & WEBX_ALTGR_MODIFIER_KEY) {
        this->handleKeySym(WEBX_KEYBOARD_KEYSYM_ALTGR, true);
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

// void WebXKeyboard::press(int key) {
//     int mappedKey  = this->getMappedKey(key);
//     if (mappedKey != 0) {
//         KeyCode keyCode = this->getMappedKeyAsKeyCode(mappedKey);
//         if (keyCode != NoSymbol) {
//             XTestFakeKeyEvent(this->_x11Display,  keyCode, True, 0);
//         }
//     }
// }

// void WebXKeyboard::release(int key) {
//     int mappedKey  = this->getMappedKey(key);
//     if (mappedKey != 0) {
//         KeyCode keyCode = this->getMappedKeyAsKeyCode(mappedKey);
//         if (keyCode != NoSymbol) {
//             XTestFakeKeyEvent(this->_x11Display,  keyCode, False, 0);
//         }
//     }
// }
