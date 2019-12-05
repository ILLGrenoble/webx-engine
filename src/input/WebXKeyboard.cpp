#include "WebXKeyboard.h"
#include <X11/extensions/XTest.h>
#include <spdlog/spdlog.h>

WebXKeyboard::WebXKeyboard(Display * x11Display) :
        _x11Display(x11Display) {
}

WebXKeyboard::~WebXKeyboard() {

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

void WebXKeyboard::press(int key) {
    int mappedKey = this->getMappedKey(key);
    if (mappedKey != 0) {
        spdlog::debug("Send fake test button event (pressed) : {0:x}", mappedKey);
        XTestFakeKeyEvent(this->_x11Display, mappedKey, True, 0);
    }
}

void WebXKeyboard::release(int key) {
    int mappedKey = this->getMappedKey(key);
    if (mappedKey != 0) {
        spdlog::debug("Send fake test button event (release): {0:x} ", mappedKey);
        XTestFakeKeyEvent(this->_x11Display, mappedKey, False, 0);
    }
}
