#ifndef WEBX_SHAPE_EVENT_H
#define WEBX_SHAPE_EVENT_H

#include <X11/Xlib.h>
#include <X11/extensions/shape.h>

class WebXShapeEvent {
public:
    /** 
     * Constructor of the WebXShapeEvent
     * @param xUnmapEvent The original XShapeEvent event
     */
    WebXShapeEvent(const XShapeEvent & xShapeEvent) :
        _xShapeEvent(xShapeEvent) {
    }

    /**
     * Destructor for the WebXShapeEvent class.
     */
    virtual ~WebXShapeEvent() {
    }

    /**
     * Gets the X11 window associated with the event.
     * @return The X11 window.
     */
    Window getWindow() const {
        return this->_xShapeEvent.window;
    }

private:
    const XShapeEvent _xShapeEvent;
};

#endif /* WEBX_SHAPE_EVENT_H */