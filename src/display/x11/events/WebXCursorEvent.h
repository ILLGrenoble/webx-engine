#ifndef WEBX_CURSOR_EVENT_H
#define WEBX_CURSOR_EVENT_H

#include <X11/Xlib.h>

class WebXCursorEvent {
public:
    /** 
     * Constructor of the WebXCursorEvent
     */
    WebXCursorEvent()  {
    }

    /**
     * Destructor for the WebXCursorEvent class.
     */
    virtual ~WebXCursorEvent() {
    }

private:
};

#endif /* WEBX_CURSOR_EVENT_H */