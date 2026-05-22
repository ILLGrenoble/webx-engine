#ifndef WEBX_SELECTION_REQUEST_EVENT_H
#define WEBX_SELECTION_REQUEST_EVENT_H

#include <X11/Xlib.h>

class WebXSelectionRequestEvent {
public:
    /** 
     * Constructor of the WebXSelectionRequestEvent
     * @param xSelectionRequestEvent The original XSelectionRequestEvent event
     */
    WebXSelectionRequestEvent(const XSelectionRequestEvent & xSelectionRequestEvent) :
        _xSelectionRequestEvent(xSelectionRequestEvent) {
    }

    /**
     * Destructor for the WebXSelectionRequestEvent class.
     */
    virtual ~WebXSelectionRequestEvent() {
    }

    /**
     * Gets the original XSelectionRequestEvent.
     * @return The XSelectionRequestEvent.
     */
    const XSelectionRequestEvent & getXSelectionRequestEvent() const {
        return this->_xSelectionRequestEvent;
    }

private:
    const XSelectionRequestEvent _xSelectionRequestEvent;
};

#endif /* WEBX_SELECTION_REQUEST_EVENT_H */