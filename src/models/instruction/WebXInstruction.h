#ifndef WEBX_INSTRUCTION_H
#define WEBX_INSTRUCTION_H

#include <string>
#include <memory>

/**
 * @class WebXInstruction
 * @brief Base class for all instructions in the WebX engine.
 * 
 * This class provides a common interface and shared attributes for all instruction types.
 */
class WebXInstruction {
public:
    /**
     * @enum Type
     * @brief Enumeration of instruction types.
     */
    enum Type {
        Windows = 2,
        Image,
        Screen,
        Mouse,
        Keyboard,
        Cursor,
        Quality,
        Pong,
        DataAck,
        Clipboard,
        Shape,
        ScreenResize,
    };

    /**
     * @brief Constructor for WebXInstruction.
     * @param type The type of the instruction.
     * @param clientId The ID of the client sending the instruction.
     * @param id The unique ID of the instruction.
     */
    WebXInstruction(Type type, uint32_t clientId, uint32_t id) :
        type(type),
        id(id),
        clientId(clientId) {
    }

    /**
     * @brief Virtual destructor for WebXInstruction.
     */
    virtual ~WebXInstruction() {}

    const Type type;
    const uint32_t clientId;
    const uint32_t id;

};

#endif /* WEBX_INSTRUCTION_H */