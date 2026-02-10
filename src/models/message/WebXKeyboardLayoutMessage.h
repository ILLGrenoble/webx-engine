#ifndef WEBX_KEYBOARD_LAYOUT_MESSAGE_H
#define WEBX_KEYBOARD_LAYOUT_MESSAGE_H

#include <memory>
#include "WebXMessage.h"

class WebXKeyboardLayoutMessage : public WebXMessage {
public:
WebXKeyboardLayoutMessage(uint64_t clientIndexMask, const std::string & keyboardLayoutName) :
        WebXMessage(Type::KeyboardLayout, clientIndexMask),
        keyboardLayoutName(keyboardLayoutName) {}
    virtual ~WebXKeyboardLayoutMessage() {}

    const std::string keyboardLayoutName;
};

#endif /* WEBX_KEYBOARD_LAYOUT_MESSAGE_H*/