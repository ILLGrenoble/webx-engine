#ifndef WEBX_WINDOW_H
#define WEBX_WINDOW_H

#include <X11/Xlib.h>
#include <X11/extensions/Xdamage.h>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <tinythread/tinythread.h>
#include <utils/WebXRectangle.h>
#include <image/WebXImageConverter.h>
#include <image/WebXImage.h>

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
        return this->_rectangle;
    }

    void setRectangle(const WebXRectangle & rectangle) {
        this->_rectangle = rectangle;
    }

    Status updateAttributes();

    void printInfo() const;

    WebXWindow * getTopParent() const;
    WebXRectangle getSubWindowRectangle() const;

    std::shared_ptr<WebXImage> getImage(WebXRectangle * subWindowRectangle, WebXRectangle * imageRectangle, WebXImageConverter * imageConverter);

    std::chrono::high_resolution_clock::time_point getImageCaptureTime() const {
        return this->_imageCaptureTime;
    }

    WebXWindow * getParent() const {
        return this->_parent;
    }

    void setParent(WebXWindow * parent) {
        this->_parent = parent;
    }

    bool parentIsRoot() const {
        return this->_parent == NULL || this->_parent->isRoot();
    }
    
    void addChild(WebXWindow * child);
    void removeChild(WebXWindow * child);
    
    const std::vector<WebXWindow *> & getChildren() const {
        return this->_children;
    }

    bool isVisible(const WebXSize & viewport) const {
        return this->_isViewable && this->_rectangle.isVisible(viewport);
    }

    void setIsViewable(bool isViewable) {
        this->_isViewable = isViewable;
    }

    void enableDamage();
    void disableDamage();

    uint32_t getWindowChecksum() const {
        return this->_windowChecksum;
    }

    uint32_t getWindowAlphaChecksum() const {
        return this->_windowAlphaChecksum;
    }

private:
    uint32_t calculateImageChecksum(std::shared_ptr<WebXImage> image);
    uint32_t calculateAlphaChecksum(std::shared_ptr<WebXImage> image);
    uint32_t getHashValue(const char * data, int len);

private:
    Display * _display;
    Window _x11Window;
    Damage _damage;
    bool _isRoot;

    WebXWindow * _parent;
    std::vector<WebXWindow *> _children;

    WebXRectangle _rectangle;
    bool _isViewable;

    std::chrono::high_resolution_clock::time_point _imageCaptureTime;

    tthread::mutex _damageMutex;

    unsigned long _windowChecksum;
    unsigned long _windowAlphaChecksum;

};


#endif /* WEBX_WINDOW_H */