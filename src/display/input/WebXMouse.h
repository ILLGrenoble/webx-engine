#ifndef WEBX_MOUSE_H
#define WEBX_MOUSE_H

#include "WebXMouseState.h"

#define LeftButtonMask          (1<<8)
#define MiddleButtonMask        (1<<9)
#define RightButtonMask         (1<<10)
#define ScrollUpButtonMask      (1<<11)
#define ScrollDownButtonMask	(1<<12)

/**
 * Represents the WebXMouse, which manages mouse state, cursor, and interactions.
 */
class WebXMouse {
public:
    /**
     * Constructor for WebXMouse.
     */
    WebXMouse() {}

    /**
     * Destructor for WebXMouse.
     */
    virtual ~WebXMouse() {}

    /**
     * Get the current mouse state.
     * @return A pointer to the WebXMouseState object.
     */
    virtual WebXMouseState * getState() const = 0;

    /**
     * Get a cursor by its ID.
     * @param cursorId The ID of the cursor (default is 0).
     * @return A shared pointer to the WebXMouseCursor object.
     */
    virtual std::shared_ptr<WebXMouseCursor> getCursor(uint32_t cursorId = 0) = 0;

    /**
     * Update the mouse position based on the current state.
     */
    virtual void updatePosition() = 0;

    /**
     * Check if the mouse position is dirty (requires client notification)
     * @return True if the mouse position is dirty, false otherwise.
     * This is used to determine if the mouse position has changed and needs to be sent to clients.
     */
    virtual bool isDirty() const = 0;

    /**
     * Set the mouse dirty state.
     * @param dirty The new dirty state.
     */
    virtual void setDirty(bool dirty) = 0;
};

#endif //WEBX_MOUSE_H
