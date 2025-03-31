#ifndef WEBX_WINDOW_VISIBILITY_H
#define WEBX_WINDOW_VISIBILITY_H

#include <X11/Xlib.h>
#include "WebXWindowCoverage.h"

/**
 * @class WebXWindowVisibility
 * @brief Represents the visibility state of an X11 window, including its coverage and viewability.
 */
class WebXWindowVisibility {
public:
    /**
     * @brief Constructs a WebXWindowVisibility object.
     * @param x11Window The X11 window handle.
     * @param rectangle The rectangle representing the window's dimensions.
     * @param isViewable Whether the window is viewable.
     */
    WebXWindowVisibility(Window x11Window, const WebXRectangle & rectangle, bool isViewable) :
        _x11Window(x11Window),
        _rectangle(rectangle),
        _isViewable(isViewable) {}
    
    /**
     * @brief Destructor for WebXWindowVisibility.
     */
    virtual ~WebXWindowVisibility() {}

    /**
     * @brief Gets the X11 window handle.
     * @return The X11 window handle.
     */
    Window getX11Window() const {
        return this->_x11Window;
    }

    /**
     * @brief Gets the coverage information of the window.
     * @return A reference to the WebXWindowCoverage object.
     */
    const WebXWindowCoverage & getCoverage() const {
        return this->_coverage;
    }

    /**
     * @brief Sets the coverage information of the window.
     * @param coverage The WebXWindowCoverage object to set.
     */
    void setCoverage(const WebXWindowCoverage & coverage) {
        this->_coverage = coverage;
    }

    /**
     * @brief Checks if the window is viewable.
     * @return True if the window is viewable, false otherwise.
     */
    bool isViewable() const {
        return this->_isViewable;
    }

    /**
     * @brief Sets the viewability of the window.
     * @param viewable True to make the window viewable, false otherwise.
     */
    void setViewable(bool viewable) {
        this->_isViewable = viewable;
    }

    /**
     * @brief Checks if the window is visible within a given viewport.
     * @param viewport The viewport dimensions.
     * @return True if the window is visible, false otherwise.
     */
    bool isVisible(const WebXSize & viewport) const {
        return this->_isViewable && this->_rectangle.isVisible(viewport);
    }

    /**
     * @brief Gets the rectangle representing the window's dimensions.
     * @return A reference to the WebXRectangle object.
     */
    const WebXRectangle & getRectangle() const {
        return this->_rectangle;
    }

    /**
     * @brief Sets the rectangle representing the window's dimensions.
     * @param rectangle The WebXRectangle object to set.
     */
    void setRectangle(const WebXRectangle & rectangle) {
        this->_rectangle = rectangle;
    }

private:
    Window _x11Window;
    WebXWindowCoverage _coverage;
    WebXRectangle _rectangle;
    bool _isViewable;

};

#endif /* WEBX_WINDOW_VISIBILITY_H */