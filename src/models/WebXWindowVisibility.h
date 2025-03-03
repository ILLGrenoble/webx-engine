#ifndef WEBX_WINDOW_VISIBILITY_H
#define WEBX_WINDOW_VISIBILITY_H

#include <X11/Xlib.h>
#include "WebXWindowCoverage.h"

class WebXWindowVisibility {
public:
    WebXWindowVisibility(Window x11Window, const WebXRectangle & rectangle, bool isViewable) :
        _x11Window(x11Window),
        _rectangle(rectangle),
        _isViewable(isViewable) {}
    virtual ~WebXWindowVisibility() {}

    Window getX11Window() const {
        return this->_x11Window;
    }

    const WebXWindowCoverage & getCoverage() const {
        return this->_coverage;
    }

    void setCoverage(const WebXWindowCoverage & coverage) {
        this->_coverage = coverage;
    }

    bool isViewable() const {
        return this->_isViewable;
    }

    void setViewable(bool viewable) {
        this->_isViewable = viewable;
    }

    bool isVisible(const WebXSize & viewport) const {
        return this->_isViewable && this->_rectangle.isVisible(viewport);
    }

    const WebXRectangle & getRectangle() const {
        return this->_rectangle;
    }

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