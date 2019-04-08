#ifndef WEBX_WINDOW_H
#define WEBX_WINDOW_H

#include <X11/Xlib.h>
#include <string>
#include <vector>
#include <memory>
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

    const std::string & getName() const {
        return this->_name;
    }

    const WebXRectangle & getRectangle() const {
        return this->_rectangle;
    }

    void setRectangle(const WebXRectangle & rectangle) {
        this->_rectangle = rectangle;
    }

    void updateAttributes();

    void printInfo() const;

    WebXWindow * getTopParent() const;
    WebXRectangle getSubWindowRectangle() const;

    void updateImage(WebXRectangle * subWindowRectangle, WebXImageConverter * imageConverter);

    std::shared_ptr<WebXImage> getImage() {
        tthread::lock_guard<tthread::mutex> lock(this->_imageMutex);
        return this->_image;
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

    bool isVisible(const WebXRectangle & viewport) const {
        return this->_isViewable && this->_rectangle.isVisible(viewport);
    }

    unsigned int getBitsPerPixel() {
        tthread::lock_guard<tthread::mutex> lock(this->_imageMutex);
        if (!this->_image) {
            return this->_image->getDepth();
        }
        return 0;
    }

    void clean();

private:
    void updateName();

private:
    Display * _display;
    Window _x11Window;
    bool _isRoot;

    WebXWindow * _parent;
    std::vector<WebXWindow *> _children;

    WebXRectangle _rectangle;
    std::string _name;
    bool _isViewable;

    tthread::mutex _imageMutex;
    std::shared_ptr<WebXImage> _image;
};


#endif /* WEBX_WINDOW_H */