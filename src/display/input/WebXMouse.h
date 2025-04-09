#ifndef WEBX_MOUSE_H
#define WEBX_MOUSE_H

#define LeftButtonMask Button1Mask
#define MiddleButtonMask Button2Mask
#define RightButtonMask Button3Mask
#define ScrollUpButtonMask Button4Mask
#define ScrollDownButtonMask Button5Mask
#define LeftButton Button1
#define MiddleButton Button2
#define RightButton Button3
#define ScrollUpButton Button4
#define ScrollDownButton Button5

#include "WebXMouseState.h"
#include "cursor/WebXMouseCursorFactory.h"

/**
 * Represents the WebXMouse, which manages mouse state, cursor, and interactions.
 */
class WebXMouse {
public:
    /**
     * Constructor for WebXMouse.
     * @param x11Display The X11 display connection.
     * @param rootWindow The root window of the X11 display.
     */
    WebXMouse(Display * x11Display, Window rootWindow);

    /**
     * Destructor for WebXMouse.
     */
    virtual ~WebXMouse();

    /**
     * Send a client instruction to update the mouse state.
     * @param x The x position.
     * @param y The y position.
     * @param buttonMask The button mask.
     */
    void sendClientInstruction(int x, int y, unsigned int buttonMask);
    
    /**
     * Get the current mouse state.
     * @return A pointer to the WebXMouseState object.
     */
    WebXMouseState * getState() const {
        return this->_state;
    }

    /**
     * Update the cursor to reflect the current state.
     */
    void updateCursor();

    /**
     * Get a cursor by its ID.
     * @param cursorId The ID of the cursor (default is 0).
     * @return A shared pointer to the WebXMouseCursor object.
     */
    std::shared_ptr<WebXMouseCursor> getCursor(uint32_t cursorId = 0);

    /**
     * Update the mouse position based on the current state.
     */
    void updatePosition();

    /**
     * Check if the mouse position is dirty (requires client notification)
     * @return True if the mouse position is dirty, false otherwise.
     * This is used to determine if the mouse position has changed and needs to be sent to clients.
     */
    bool isDirty() const {
        return this->_isDirty;
    }

    /**
     * Set the mouse dirty state.
     * @param dirty The new dirty state.
     */
    void setDirty(bool dirty) {
        this->_isDirty = dirty;
    }

private:
    /**
     * Create the default mouse state.
     * @return A pointer to the newly created WebXMouseState object.
     */
    WebXMouseState * createDefaultMouseState();

    /**
     * Send mouse button events based on the new button mask.
     * @param newButtonMask The new button mask.
     */
    void sendMouseButtons(unsigned int newButtonMask);

    /**
     * Send a single mouse button event.
     * @param button The button to send.
     * @param isPressed Whether the button is pressed or released.
     */
    void sendMouseButton(unsigned int button, Bool isPressed);

    /**
     * Send mouse movement events.
     * @param newX The new x position.
     * @param newY The new y position.
     */
    void sendMouseMovement(int newX, int newY);

private:
    Display * _x11Display;
    Window  _rootWindow;
    WebXMouseCursorFactory _cursorFactory;
    WebXMouseState * _state;
    bool _isDirty;
};

#endif //WEBX_MOUSE_H
