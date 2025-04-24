#ifndef WEBX_MAP_EVENT_H
#define WEBX_MAP_EVENT_H

#include <X11/Xlib.h>

class WebXMapEvent {
public:
    /** 
     * Constructor of the WebXMapEvent
     * @param xMapEvent The original XMapEvent event
     */
    WebXMapEvent(const XMapEvent & xMapEvent) :
        _xMapEvent(xMapEvent) {
    }

    /**
     * Destructor for the WebXMapEvent class.
     */
    virtual ~WebXMapEvent() {
    }

    /**
     * Gets the X11 window associated with the event.
     * @return The X11 window.
     */
    Window getWindow() const {
        return this->_xMapEvent.window;
    }

    /**
     * Returns the serial associated with the event.
     * @return the serial of the event
     */
    int getSerial() const {
        return this->_xMapEvent.serial;
    }

private:
    const XMapEvent _xMapEvent;
};

#endif /* WEBX_MAP_EVENT_H */