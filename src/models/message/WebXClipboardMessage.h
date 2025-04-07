#ifndef WEBX_CLIPBOARD_MESSAGE_H
#define WEBX_CLIPBOARD_MESSAGE_H

#include <vector>
#include <memory>
#include "WebXMessage.h"

class WebXClipboardMessage : public WebXMessage {
public:
WebXClipboardMessage(uint64_t clientIndexMask, const std::string & clipboardContent) :
        WebXMessage(Type::Clipboard, clientIndexMask),
        clipboardContent(clipboardContent) {}
    virtual ~WebXClipboardMessage() {}

    const std::string clipboardContent;
};

#endif /* WEBX_CLIPBOARD_MESSAGE_H*/