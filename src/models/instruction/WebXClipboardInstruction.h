#ifndef WEBX_CLIPBOARD_INSTRUCTION_H
#define WEBX_CLIPBOARD_INSTRUCTION_H

#include <vector>
#include <memory>
#include "WebXInstruction.h"

class WebXClipboardInstruction : public WebXInstruction {
public:
    WebXClipboardInstruction(uint32_t clientId, uint32_t id, const std::string & clipboardContent) :
        WebXInstruction(Type::Clipboard, clientId, id),
        clipboardContent(clipboardContent) {}
    virtual ~WebXClipboardInstruction() {}

    const std::string clipboardContent;
};

#endif /* WEBX_CLIPBOARD_INSTRUCTION_H */