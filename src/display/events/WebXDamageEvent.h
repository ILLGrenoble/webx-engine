#ifndef WEBX_DAMAGE_EVENT_H
#define WEBX_DAMAGE_EVENT_H

#include <X11/Xlib.h>
#include <X11/extensions/Xdamage.h>
#include <models/WebXRectangle.h>

class WebXDamageEvent {
public:
    /** 
     * Constructor of the WebXDamageEvent
     * @param xDamageEvent The original XDamageNotify event
     */
    WebXDamageEvent(const XDamageNotifyEvent & xDamageEvent) :
        _xDamageEvent(xDamageEvent) {
    }

    /**
     * Destructor for the WebXDamageEvent class.
     */
    virtual ~WebXDamageEvent() {
    }

    /**
     * Gets the X11 window associated with the event.
     * @return The X11 window.
     */
    Window getWindow() const {
        return this->_xDamageEvent.drawable;
    }

    /**
     * Returns the serial associated with the event.
     * @return the serial of the event
     */
    int getSerial() const {
        return this->_xDamageEvent.serial;
    }

    /**
     * Gets the rectangle of the damage
     * @return The rectangle of the damage.
     */
    WebXRectangle getRectangle() const {
        const XRectangle & area = this->_xDamageEvent.area;
        return WebXRectangle(area.x, area.y, area.width, area.height);
    }

    private:
    const XDamageNotifyEvent _xDamageEvent;
};

#endif /* WEBX_DAMAGE_EVENT_H */