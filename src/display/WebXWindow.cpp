#include "WebXWindow.h"
#include <image/WebXImage.h>
#include <algorithm>

WebXWindow::WebXWindow(Display * display, Window x11Window, bool isRoot, int x, int y, int width, int height, bool isViewable) :
    _display(display),
    _x11Window(x11Window),
    _damage(0),
    _isRoot(isRoot),
    _parent(NULL),
    _rectangle(WebXRectangle(x, y, width, height)),
    _isViewable(isViewable),
    _imageCaptureTime(std::chrono::high_resolution_clock::now()),
    _isDamaged(false) {

    this->updateName();
}

WebXWindow::~WebXWindow() {
    this->disableDamage();
}

void WebXWindow::enableDamage() {
    tthread::lock_guard<tthread::mutex> lock(this->_damageMutex);
    if (this->_damage == 0) {
        this->_damage = XDamageCreate(this->_display, this->_x11Window, XDamageReportBoundingBox);
    }
}

void WebXWindow::disableDamage() {
    tthread::lock_guard<tthread::mutex> lock(this->_damageMutex);
    if (this->_damage != 0) {
        XDamageDestroy(this->_display, this->_damage);
        this->_damage = 0;
    }
}

void WebXWindow::setDamaged(const WebXRectangle & area) {
    tthread::lock_guard<tthread::mutex> lock(this->_damageMutex);
    if (this->_isDamaged) {
        this->_damageRectangle.combine(area);

    } else {
        this->_isDamaged = true;
        this->_damageRectangle = area;
    }
    // printf("Damage rectangle for window 0x%0lx = [%d %d %d %d]\n", this->_x11Window, 
    //     this->_damageRectangle.x, this->_damageRectangle.y, this->_damageRectangle.width, this->_damageRectangle.height);
}

void WebXWindow::clearDamaged() {
    tthread::lock_guard<tthread::mutex> lock(this->_damageMutex);
    this->_isDamaged = false;
    this->_damageRectangle.clear();
    if (this->_damage != 0) {
        XDamageSubtract(this->_display, this->_damage, None, None);
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
    tthread::lock_guard<tthread::mutex> lock(this->_imageMutex);
    WebXRectangle rectangle = this->getRectangle();
    // printf("Grabbing WebXWindow = 0x%08lx [(%d, %d), %dx%d]:\n", this->_x11Window, rectangle.x, rectangle.y, rectangle.width, rectangle.height);
    XImage * image = XGetImage(this->_display, this->_x11Window, 0, 0, rectangle.width, rectangle.height, AllPlanes, ZPixmap);

    if (!image) {
        printf("Failed to get image for window 0x%08lx %d %d\n", this->_x11Window, rectangle.width, rectangle.height);

    } else {
        this->_image = std::shared_ptr<WebXImage>(imageConverter->convert(image, subWindowRectangle));
    }
    this->_imageCaptureTime = std::chrono::high_resolution_clock::now();
    this->clearDamaged();
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

