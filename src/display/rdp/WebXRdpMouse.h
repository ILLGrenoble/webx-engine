#ifndef WEBX_RDP_MOUSE_H
#define WEBX_RDP_MOUSE_H

#include <display/input/WebXMouse.h>

class WebXMouseState;
class WebXRdpClient;

/**
 * Represents the WebXRdpMouse, which manages mouse state, cursor, and interactions.
 */
class WebXRdpMouse : public WebXMouse {
public:
    /**
     * Constructor for WebXRdpMouse.
     */
    WebXRdpMouse(WebXRdpClient * rdpClient);

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
    /**
     * Create the default mouse state.
     * @return A pointer to the newly created WebXx11MouseState object.
     */
    WebXMouseState * createDefaultMouseState();

private:
    WebXRdpClient * _rdpClient;
    WebXMouseState * _state;
    bool _isDirty;
};

#endif //WEBX_RDP_MOUSE_H
