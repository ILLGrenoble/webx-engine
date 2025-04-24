#ifndef WEBX_CONFIGURE_EVENT_H
#define WEBX_CONFIGURE_EVENT_H

#include <X11/Xlib.h>

class WebXConfigureEvent {
public:
    /** 
     * Constructor of the WebXConfigureEvent
     * @param xConfigureEvent The original XConfigureEvent event
     */
    WebXConfigureEvent(const XConfigureEvent & xConfigureEvent) :
        _xConfigureEvent(xConfigureEvent) {
    }

    /**
     * Destructor for the WebXConfigureEvent class.
     */
    virtual ~WebXConfigureEvent() {
    }

    /**
     * Gets the X11 window associated with the event.
     * @return The X11 window.
     */
    Window getWindow() const {
        return this->_xConfigureEvent.window;
    }

    /**
     * Returns the serial associated with the event.
     * @return the serial of the event
     */
    int getSerial() const {
        return this->_xConfigureEvent.serial;
    }

    /**
     * Gets the x coordinate of the window
     * @return The x coordinate of the window.
     */
    int getX() const {
        return this->_xConfigureEvent.x;
    }

    /**
     * Gets the y coordinate of the window
     * @return The y coordinate of the window.
     */
    int getY() const {
        return this->_xConfigureEvent.y;
    }

    /**
     * Gets the width of the window
     * @return The width of the window.
     */
    int getWidth() const {
        return this->_xConfigureEvent.width;
    }

    /**
     * Gets the height of the window
     * @return The height of the window.
     */
    int getHeight() const {
        return this->_xConfigureEvent.height;
    }

private:
    const XConfigureEvent _xConfigureEvent;
};

#endif /* WEBX_CONFIGURE_EVENT_H */