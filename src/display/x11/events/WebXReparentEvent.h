#ifndef WEBX_REPARENT_EVENT_H
#define WEBX_REPARENT_EVENT_H

#include <X11/Xlib.h>

class WebXReparentEvent {
public:
    /** 
     * Constructor of the WebXReparentEvent
     * @param xReparentEvent The original XReparentEvent event
     */
    WebXReparentEvent(const XReparentEvent & xReparentEvent) :
        _xReparentEvent(xReparentEvent) {
    }

    /**
     * Destructor for the WebXReparentEvent class.
     */
    virtual ~WebXReparentEvent() {
    }

    /**
     * Gets the X11 window associated with the event.
     * @return The X11 window.
     */
    Window getWindow() const {
        return this->_xReparentEvent.window;
    }

    /**
     * Returns the serial associated with the event.
     * @return the serial of the event
     */
    int getSerial() const {
        return this->_xReparentEvent.serial;
    }

    /**
     * Gets the X11 parent window associated with the event.
     * @return The X11 parent window.
     */
    Window getParentWindow() const {
        return this->_xReparentEvent.parent;
    }

private:
    const XReparentEvent _xReparentEvent;
};

#endif /* WEBX_REPARENT_EVENT_H */