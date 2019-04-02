#include "WebXWindow.h"
#include <image/WebXImage.h>
#include <algorithm>

WebXWindow::WebXWindow(Display * display, Window x11Window, bool isRoot, int x, int y, int width, int height, bool isViewable) :
    _display(display),
    _x11Window(x11Window),
    _isRoot(isRoot),
    _parent(NULL),
    _rectangle(WebXRectangle(x, y, width, height)),
    _isViewable(isViewable),
    _image(NULL) {

    this->updateName();
}

WebXWindow::~WebXWindow() {
    this->clean();
}

void WebXWindow::clean() {
    if (this->_image != NULL) {
        delete this->_image;
        this->_image = NULL;
    }
}

void WebXWindow::updateName() {
    Atom atom = XInternAtom(this->_display, "_NET_WM_NAME", False);
    Atom type;
    int form;
    unsigned long remain, len;
    unsigned char *list;

    if (Success != XGetWindowProperty(this->_display, this->_x11Window, atom, 0, 1024, False, AnyPropertyType, &type, &form, &len, &remain, &list)) {
        printf("Couldn't access window list\n");
    }
    XFree(list);

    char * charName = NULL;
    XFetchName(this->_display, this->_x11Window, &charName);

    if (charName != NULL) {
        std::string name = std::string(charName);
        XFree(charName);
        this->_name = name;
    
    } else {
        this->_name = "NULL";
    }
}

void WebXWindow::updateAttributes() {
    XWindowAttributes attr;
    Status status = XGetWindowAttributes(this->_display, this->_x11Window, &attr);
    if (status != BadWindow) {
        this->_rectangle = WebXRectangle(attr.x, attr.y, attr.width, attr.height);
        this->_isViewable = (attr.map_state == IsViewable);
    } else {
        this->_rectangle = WebXRectangle(0, 0, -1, -1);
        this->_isViewable = false;
    }

    this->updateName();
}

void WebXWindow::printInfo() const {
    printf("WebXWindow = 0x%08lx [(%d, %d), %dx%d] %s\n", this->_x11Window, this->_rectangle.x, this->_rectangle.y, this->_rectangle.width, this->_rectangle.height, this->_name.c_str());
}


WebXWindow * WebXWindow::getTopParent() const {
    WebXWindow * child = (WebXWindow *)this;
    while (!child->parentIsRoot()) {
        child = this->getParent();
    }

    return child;
}

WebXRectangle WebXWindow::getSubWindowRectangle() const {
    const WebXRectangle & initialRectangle = this->getRectangle();
    const WebXWindow * child = this;
    WebXRectangle subWindowRectangle(0, 0, initialRectangle.width, initialRectangle.height);

    while (!child->parentIsRoot()) {
        const WebXRectangle & childRectangle = child->getRectangle();
        subWindowRectangle.x += childRectangle.x;
        subWindowRectangle.y += childRectangle.y;

        child = child->getParent();
    }

    return subWindowRectangle;
}

void WebXWindow::updateImage(WebXRectangle * subWindowRectangle, WebXImageConverter * imageConverter) {

    if (this->_image != NULL) {
        delete this->_image;
        this->_image = NULL;
    }

    WebXRectangle geometry = this->getRectangle();
    // printf("Grabbing WebXWindow = 0x%08lx [(%d, %d), %dx%d]:\n", this->_x11Window, geometry.x, geometry.y, geometry.width, geometry.height);
    XImage * image = XGetImage(this->_display, this->_x11Window, 0, 0, geometry.width, geometry.height, AllPlanes, ZPixmap);

    if (!image) {
        printf("Failed to get image for window 0x%08lx\n", this->_x11Window);

    } else {
        this->_image = imageConverter->convert(image, subWindowRectangle);
    }
}

void WebXWindow::addChild(WebXWindow * child) {
    std::vector<WebXWindow *>::iterator it = find(this->_children.begin(), this->_children.end(), child);
    if (it == this->_children.end()) {
        this->_children.push_back(child);
        child->setParent(this);
    }
}

void WebXWindow::removeChild(WebXWindow * child) {
    std::vector<WebXWindow *>::iterator it = find(this->_children.begin(), this->_children.end(), child);
    if (it != this->_children.end()) {
        this->_children.erase(it);

        child->setParent(NULL);
    }
}

