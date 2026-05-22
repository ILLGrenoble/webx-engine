#ifndef WEBX_SELECTION_EVENT_H
#define WEBX_SELECTION_EVENT_H

#include <X11/Xlib.h>

class WebXSelectionEvent {
public:
    /** 
     * Constructor of the WebXSelectionEvent
     * @param xSelectionEvent The original XSelectionEvent event
     */
    WebXSelectionEvent(const XSelectionEvent & xSelectionEvent) :
        _xSelectionEvent(xSelectionEvent) {
    }

    /**
     * Destructor for the WebXSelectionEvent class.
     */
    virtual ~WebXSelectionEvent() {
    }

    /**
     * Gets the original X11 event
     * @return The original X11 event.
     */
    const XSelectionEvent & getXSelectionEvent() const {
        return this->_xSelectionEvent;
    }

private:
    const XSelectionEvent _xSelectionEvent;
};

#endif /* WEBX_SELECTION_EVENT_H */