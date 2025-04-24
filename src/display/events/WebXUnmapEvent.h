#ifndef WEBX_UNMAP_EVENT_H
#define WEBX_UNMAP_EVENT_H

#include <X11/Xlib.h>

class WebXUnmapEvent {
public:
    /** 
     * Constructor of the WebXUnmapEvent
     * @param xUnmapEvent The original XUnmapEvent event
     */
    WebXUnmapEvent(const XUnmapEvent & xUnmapEvent) :
        _xUnmapEvent(xUnmapEvent) {
    }

    /**
     * Destructor for the WebXUnmapEvent class.
     */
    virtual ~WebXUnmapEvent() {
    }

    /**
     * Gets the X11 window associated with the event.
     * @return The X11 window.
     */
    Window getWindow() const {
        return this->_xUnmapEvent.window;
    }

private:
    const XUnmapEvent _xUnmapEvent;
};

#endif /* WEBX_UNMAP_EVENT_H */