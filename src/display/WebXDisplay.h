#ifndef WEBX_DISPLAY_H
#define WEBX_DISPLAY_H

#include <X11/Xlib.h>
#include <map>
#include <vector>
#include <utils/WebXSize.h>
#include <tinythread/tinythread.h>
#include "WebXWindowProperties.h"

class WebXWindow;
class WebXImageConverter;

class WebXDisplay {
public:
    WebXDisplay(Display * display);
    virtual ~WebXDisplay();

    void init();

    const WebXSize & getScreenSize() const {
        return this->_screenSize;
    }

    WebXWindow * getWindow(Window window) const;
    WebXWindow * createWindowInTree(Window x11Window);

    void removeWindowFromTree(Window x11Window);
    void reparentWindow(Window x11Window, Window parentX11Window);

    const std::vector<WebXWindow *> & getVisibleWindows() {
        tthread::lock_guard<tthread::mutex> lock(this->_visibleWindowsMutex);
        return this->_visibleWindows;
    }

    const std::vector<WebXWindowProperties> & getVisibleWindowsProperties() {
        tthread::lock_guard<tthread::mutex> lock(this->_visibleWindowsMutex);
        return this->_visibleWindowsProperties;
    }

    WebXWindow * getRootWindow() const {
        return this->_rootWindow;
    }

    void updateVisibleWindows();
    void debugTree(Window root = 0, int indent = 0);

    WebXWindow * getManagedWindow(const WebXWindow * window) const {
        std::map<const WebXWindow *, WebXWindow *>::const_iterator it = this->_managedWindows.find(window);
        if (it != this->_managedWindows.end()) {
            return it->second;
        }
        return NULL;
    }

    void updateImage(WebXWindow * window) const;

private:
    struct WebXTreeDetails {
        WebXTreeDetails() :
            root(0),
            parent(0),
            children(NULL),
            numberOfChildren(0) {
        }
        ~WebXTreeDetails() {
            if (children != NULL) {
                XFree(children);
            }
        }

        Window root;
        Window parent;
        Window * children;
        unsigned int numberOfChildren;
    };

    bool queryTree(Display * display, Window window, WebXTreeDetails & tree) const {
        return XQueryTree(display, window, &tree.root, &tree.parent, &tree.children, &tree.numberOfChildren);
    }

private:
    void updateManagedWindows();
    WebXWindow * createWindow(Window x11Window, bool isRoot = false);
    void deleteWindow(WebXWindow * window);
    void createTree(WebXWindow * root);
    void deleteTree(WebXWindow * root);
    WebXWindow * getParent(WebXWindow * window);

private:
    Display * _x11Display;
    WebXSize _screenSize;

    WebXWindow * _rootWindow;
    std::map<Window, WebXWindow *> _allWindows;
    std::map<const WebXWindow *, WebXWindow *> _managedWindows;
    std::vector<WebXWindow *> _visibleWindows;
    std::vector<WebXWindowProperties> _visibleWindowsProperties;

    tthread::mutex _visibleWindowsMutex;

    WebXImageConverter * _imageConverter;
};


#endif /* WEBX_DISPLAY_H */