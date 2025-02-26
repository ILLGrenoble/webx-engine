#ifndef WEBX_QUALITY_INSTRUCTION_H
#define WEBX_QUALITY_INSTRUCTION_H

#include "WebXInstruction.h"

class WebXQualityInstruction : public WebXInstruction {

public:
    WebXQualityInstruction(uint32_t clientId, uint32_t id, uint32_t qualityIndex) :
        WebXInstruction(Type::Quality, clientId, id),
        qualityIndex(qualityIndex) {
    }

    virtual ~WebXQualityInstruction() {
    }

    uint32_t qualityIndex;
};


#endif //WEBX_QUALITY_INSTRUCTION_H
