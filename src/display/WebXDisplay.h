#ifndef WEBX_DISPLAY_H
#define WEBX_DISPLAY_H

#include <X11/Xlib.h>
#include <map>
#include <vector>
#include <memory>
#include <utils/WebXSize.h>
#include <thread>
#include <mutex>
#include "WebXWindowProperties.h"
#include "WebXWindowDamageProperties.h"
#include <utils/WebXQuality.h>

class WebXWindow;
class WebXImageConverter;
class WebXMouse;
class WebXKeyboard;

class WebXDisplay {
public:
    WebXDisplay(Display * display);
    virtual ~WebXDisplay();

    void init();

    const WebXSize & getScreenSize() const {
        return this->_screenSize;
    }

    WebXWindow * getWindow(Window window) const;
    WebXWindow * getVisibleWindow(Window window);
    WebXWindow * createWindowInTree(Window x11Window);

    void removeWindowFromTree(Window x11Window);
    void reparentWindow(Window x11Window, Window parentX11Window);

    const std::vector<WebXWindow *> & getVisibleWindows() {
        const std::lock_guard<std::mutex> lock(this->_visibleWindowsMutex);
        return this->_visibleWindows;
    }

    const std::vector<WebXWindowProperties> getVisibleWindowsProperties() {
        const std::lock_guard<std::mutex> lock(this->_visibleWindowsMutex);
        std::vector<WebXWindowProperties> visibleWindowsProperties;

        std::transform(this->_visibleWindows.begin(), this->_visibleWindows.end(), std::back_inserter(visibleWindowsProperties), [](WebXWindow * window) { 
            return WebXWindowProperties(window);
        });

        return visibleWindowsProperties;
    }

    WebXWindow * getRootWindow() const {
        return this->_rootWindow;
    }

    void updateVisibleWindows();
    void debugTree(Window root = 0, int indent = 0);

    std::shared_ptr<WebXImage> getImage(Window x11Window, const WebXQuality & quality, WebXRectangle * imageRectangle = NULL);

    void addDamagedWindow(Window x11Window, const WebXRectangle & damagedArea, bool fullWindowRefresh = false);
    std::vector<WebXWindowDamageProperties> getDamagedWindows(const WebXQuality & quality);
    
    WebXMouse * getMouse() const {
        return this->_mouse;
    }

    void updateMouseCursor();

    void sendClientMouseInstruction(int x, int y, unsigned int buttonMask);
    void sendKeyboard(int keysym, bool pressed);
    void loadKeyboardLayout(const std::string & layout);
    
    void onImageDataSent(Window x11Window, float imageSizeKB);

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
    WebXWindow * createWindow(Window x11Window, bool isRoot = false);
    void deleteWindow(WebXWindow * window);
    void createTree(WebXWindow * root);
    void deleteTree(WebXWindow * root);
    WebXWindow * getParent(WebXWindow * window);
    void updateWindowCoverage();

private:
    Display * _x11Display;
    WebXSize _screenSize;

    WebXWindow * _rootWindow;
    std::map<Window, WebXWindow *> _allWindows;

    std::vector<WebXWindow *> _visibleWindows;
    std::mutex _visibleWindowsMutex;

    WebXImageConverter * _imageConverter;

    std::vector<WebXWindowDamageProperties> _damagedWindows;
    std::mutex _damagedWindowsMutex;

    WebXMouse * _mouse;
    WebXKeyboard * _keyboard;
};


#endif /* WEBX_DISPLAY_H */