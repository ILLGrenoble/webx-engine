#ifndef WEBX_WINDOW_H
#define WEBX_WINDOW_H

#include <X11/Xlib.h>
#include <X11/extensions/Xdamage.h>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <mutex>
#include <utils/WebXRectangle.h>
#include <utils/WebXQuality.h>
#include <image/WebXImageConverter.h>
#include <image/WebXImage.h>
#include <models/WebXWindowCoverage.h>
#include <models/WebXWindowVisibility.h>

class WebXWindow {

public:
    WebXWindow(Display * display, Window window, bool isRoot, int x, int y, int width, int height, bool isViewable);
    virtual ~WebXWindow();

    Window getX11Window() const {
        return this->_x11Window;
    } 

    bool isRoot() const {
        return this->_isRoot;
    }

    const WebXRectangle & getRectangle() const {
        return this->_visibility.getRectangle();
    }

    void setRectangle(const WebXRectangle & rectangle) {
        this->_visibility.setRectangle(rectangle);
    }

    Status updateAttributes();

    void printInfo() const;

    std::shared_ptr<WebXImage> getImage(const WebXRectangle * imageRectangle, WebXImageConverter * imageConverter, const WebXQuality & requestedQuality);

    WebXWindow * getParent() const {
        return this->_parent;
    }

    void setParent(WebXWindow * parent) {
        this->_parent = parent;
    }

    void addChild(WebXWindow * child);
    void removeChild(WebXWindow * child);
    
    const std::vector<WebXWindow *> & getChildren() const {
        return this->_children;
    }

    bool isVisible(const WebXSize & viewport) const {
        return this->_visibility.isVisible(viewport);
    }

    void setIsViewable(bool isViewable) {
        this->_visibility.setViewable(isViewable);
    }

    bool isViewable() const {
        return this->_visibility.isViewable();
    }

    void setCoverage(const WebXWindowCoverage & coverage) {
        this->_visibility.setCoverage(coverage);
    }

    const WebXWindowVisibility & getVisibility() const {
        return this->_visibility;
    }
    
    void enableDamage();
    void disableDamage();

private:
    Display * _display;
    Window _x11Window;
    Damage _damage;
    bool _isRoot;

    WebXWindow * _parent;
    std::vector<WebXWindow *> _children;

    WebXWindowVisibility _visibility;

    std::mutex _damageMutex;
};


#endif /* WEBX_WINDOW_H */