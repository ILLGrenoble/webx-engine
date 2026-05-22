#ifndef WEBX_RANDR_EVENT_H
#define WEBX_RANDR_EVENT_H

#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

class WebXRandREvent {
public:
    /** 
     * Constructor of the WebXRandREvent
     * @param screenChangeEvent The original XRRScreenChangeNotifyEvent event
     */
    WebXRandREvent(const XRRScreenChangeNotifyEvent & screenChangeEvent) :
        _screenChangeEvent(screenChangeEvent) {
    }

    /**
     * Destructor for the WebXRandREvent class.
     */
    virtual ~WebXRandREvent() {
    }

    /**
     * Gets the screen width
     * @return The screen width.
     */
    int getWidth() const {
        return this->_screenChangeEvent.width;
    }

    /**
     * Gets the screen height
     * @return The screen height.
     */
    int getHeight() const {
        return this->_screenChangeEvent.height;
    }

    /**
     * Get the raw event
     */
    XRRScreenChangeNotifyEvent * event() const {
        return (XRRScreenChangeNotifyEvent *)&this->_screenChangeEvent;
    }

private:
    const XRRScreenChangeNotifyEvent _screenChangeEvent;
};

#endif /* WEBX_RANDR_EVENT_H */