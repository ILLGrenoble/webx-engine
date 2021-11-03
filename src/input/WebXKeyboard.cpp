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
    std::string mappingName = "fr";

    // Dummy init of keyboard. Using French keyboard symbol mapping
    auto it = std::find_if(begin(WEBX_KEY_MAPS), end(WEBX_KEY_MAPS), [=](const WebXKeyboardMapping & mapping) {
        return mapping.name == mappingName;
    });

    if (it != WEBX_KEY_MAPS.end()) {
        auto keyboardMapping = *it;
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


KeyCode WebXKeyboard::getMappedKeyAsKeyCode(int mappedKey) const {
    return XKeysymToKeycode(this->_x11Display, mappedKey);
}

void WebXKeyboard::handleKeySym(int keysym, bool pressed) {
    if (this->_keyboardMapping == NULL) {
        return;
    }
    int mappedKey = this->getMappedKey(keysym);
}


void WebXKeyboard::press(int key) {
    int mappedKey  = this->getMappedKey(key);
    if (mappedKey != 0) {
        KeyCode keyCode = this->getMappedKeyAsKeyCode(mappedKey);
        if (keyCode != NoSymbol) {
            XTestFakeKeyEvent(this->_x11Display,  keyCode, True, 0);
        }
    }
}

void WebXKeyboard::release(int key) {
    int mappedKey  = this->getMappedKey(key);
    if (mappedKey != 0) {
        KeyCode keyCode = this->getMappedKeyAsKeyCode(mappedKey);
        if (keyCode != NoSymbol) {
            XTestFakeKeyEvent(this->_x11Display,  keyCode, False, 0);
        }
    }
}
