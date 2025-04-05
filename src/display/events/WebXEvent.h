#ifndef WEBX_EVENT_H
#define WEBX_EVENT_H

#include <X11/Xlib.h>
#include <models/WebXRectangle.h>

/**
 * Enum representing the different types of WebX events.
 */
enum WebXEventType {
    Create = 0,
    Destroy,
    Map,
    Unmap,
    Reparent,
    Configure,
    Gravity,
    Circulate,
    Damaged,
    Other,
    MouseCursor,
    ClipboardNotify,
};

/**
 * The WebXEvent class encapsulates an X11 event and provides additional
 * information such as the event type and geometry.
 */
class WebXEvent {
public:
    /**
     * Constructor to initialize a WebXEvent from an X11 event.
     * @param xEvent The X11 event.
     * @param damageEventBase Base event number for damage events.
     * @param fixesEventBase Base event number for XFixes events.
     */
    WebXEvent(const XEvent & xEvent, int damageEventBase, int fixesEventBase);

    /**
     * Destructor for the WebXEvent class.
     */
    virtual ~WebXEvent();

    /**
     * Gets the X11 window associated with the event.
     * @return The X11 window.
     */
    Window getX11Window() const {
        return this->_x11Window;
    }

    /**
     * Gets the type of the event.
     * @return The event type.
     */
    WebXEventType getType() const {
        return this->_type;
    }

    /**
     * Gets the X coordinate of the event.
     * @return The X coordinate.
     */
    int getX() const {
        return this->_x;
    }

    /**
     * Gets the Y coordinate of the event.
     * @return The Y coordinate.
     */
    int getY() const {
        return this->_y;
    }

    /**
     * Gets the width of the event's geometry.
     * @return The width.
     */
    int getWidth() const {
        return this->_width;
    }

    /**
     * Gets the height of the event's geometry.
     * @return The height.
     */
    int getHeight() const {
        return this->_height;
    }

    /**
     * Gets the parent window of the event's window.
     * @return The parent window.
     */
    Window getParent() const {
        return this->_parent;
    }

    /**
     * Gets the rectangle representing the event's geometry.
     * @return A WebXRectangle object.
     */
    WebXRectangle getRectangle() const {
        return WebXRectangle(this->_x, this->_y, this->_width, this->_height);
    }

    /**
     * Gets the selection property associated with the Selection event.
     * @return The selection property.
     */
    Atom getSelectionProperty() const {
        return this->_selectionProperty;
    }

private:
    /**
     * Converts the X11 event into a WebXEvent by extracting relevant information.
     */
    void convert();

private:
    XEvent _xEvent;
    Window _x11Window;
    WebXEventType _type;

    int _x;
    int _y;
    int _width;
    int _height;
    Atom _selectionProperty;
    Window _parent;
    int _damageEventBase;
    int _fixesEventBase;
};

#endif /* WEBX_EVENT_H */