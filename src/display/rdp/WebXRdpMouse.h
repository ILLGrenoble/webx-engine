#ifndef WEBX_RDP_MOUSE_H
#define WEBX_RDP_MOUSE_H

#include <display/input/WebXMouse.h>

class WebXMouseState;
class WebXRdpPointer;

#define LeftButton PTR_FLAGS_BUTTON1
#define MiddleButton PTR_FLAGS_BUTTON3
#define RightButton PTR_FLAGS_BUTTON2
#define ScrollUpButton PTR_FLAGS_WHEEL | 0x78
#define ScrollDownButton PTR_FLAGS_WHEEL | PTR_FLAGS_WHEEL_NEGATIVE | 0x88

/**
 * Represents the WebXRdpMouse, which manages mouse state, cursor, and interactions.
 */
class WebXRdpMouse : public WebXMouse {
public:
    /**
     * Constructor for WebXRdpMouse.
     */
    WebXRdpMouse(WebXRdpPointer * rdpPointer);

    /**
     * Destructor for WebXRdpMouse.
     */
    virtual ~WebXRdpMouse();

    /**
     * Send a client instruction to update the mouse state.
     * @param x The x position.
     * @param y The y position.
     * @param buttonMask The button mask.
     */
    void sendClientInstruction(int x, int y, unsigned int buttonMask);
    
    /**
     * Get the current mouse state.
     * @return A pointer to the WebXRdpMouseState object.
     */
    virtual WebXMouseState * getState() const {
        return this->_state;
    }

    /**
     * Get a cursor by its ID.
     * @param cursorId The ID of the cursor (default is 0).
     * @return A shared pointer to the WebXRdpMouseCursor object.
     */
    virtual std::shared_ptr<WebXMouseCursor> getCursor(uint32_t cursorId = 0);

    void updateCursor();

    /**
     * Update the mouse position based on the current state.
     */
    virtual void updatePosition();

    /**
     * Check if the mouse position is dirty (requires client notification)
     * @return True if the mouse position is dirty, false otherwise.
     * This is used to determine if the mouse position has changed and needs to be sent to clients.
     */
    virtual bool isDirty() const {
        return this->_isDirty;
    }

    /**
     * Set the mouse dirty state.
     * @param dirty The new dirty state.
     */
    virtual void setDirty(bool dirty) {
        this->_isDirty = dirty;
    }

private: 
    void sendMouseButtons(unsigned int newButtonMask);
    void sendMouseButton(unsigned int button, bool isPressed);
    void sendMouseMovement(int newX, int newY);

    /**
     * Create the default mouse state.
     * @return A pointer to the newly created WebXx11MouseState object.
     */
    WebXMouseState * createDefaultMouseState();

private:
    WebXRdpPointer * _rdpPointer;
    WebXMouseState * _state;
    bool _isDirty;
};

#endif //WEBX_RDP_MOUSE_H
