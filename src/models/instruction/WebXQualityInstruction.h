#ifndef WEBX_QUALITY_INSTRUCTION_H
#define WEBX_QUALITY_INSTRUCTION_H

#include "WebXInstruction.h"

/**
 * @class WebXQualityInstruction
 * @brief Represents an instruction specific to quality settings.
 * 
 * This class inherits from WebXInstruction and is used to handle
 * instructions related to quality settings. It includes an additional
 * qualityIndex parameter to specify the quality level.
 */
class WebXQualityInstruction : public WebXInstruction {

public:
    /**
     * @brief Constructor for WebXQualityInstruction.
     * @param clientId The ID of the client sending the instruction.
     * @param id The unique ID of the instruction.
     * @param qualityIndex The quality index for the instruction.
     */
    WebXQualityInstruction(uint32_t clientId, uint32_t id, uint32_t qualityIndex) :
        WebXInstruction(Type::Quality, clientId, id),
        qualityIndex(qualityIndex) {
    }

    /**
     * @brief Destructor for WebXQualityInstruction.
     */
    virtual ~WebXQualityInstruction() {
    }

    uint32_t qualityIndex; ///< The quality index for the instruction.
};

#endif //WEBX_QUALITY_INSTRUCTION_H
