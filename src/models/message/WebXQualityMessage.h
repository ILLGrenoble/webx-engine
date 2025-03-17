#ifndef WEBX_QUALITY_MESSAGE_H
#define WEBX_QUALITY_MESSAGE_H

#include "WebXMessage.h"
#include <models/WebXQuality.h>

class WebXQualityMessage : public WebXMessage {
public:
    WebXQualityMessage(uint64_t clientIndexMask, const WebXQuality & quality) :
        WebXMessage(Type::Quality, clientIndexMask),
        quality(quality) {}
    virtual ~WebXQualityMessage() {}

    const WebXQuality & quality;
};


#endif /* WEBX_QUALITY_MESSAGE_H */