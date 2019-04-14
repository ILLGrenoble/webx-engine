#include "WebXDisplay.h"
#include "WebXWindow.h"
#include <image/WebXPNGImageConverter.h>
#include <algorithm>
#include <X11/Xatom.h>

WebXDisplay::WebXDisplay(Display * display) :
    _x11Display(display),
    _rootWindow(NULL),
    _imageConverter(NULL) {

}

WebXDisplay::~WebXDisplay() {
    this->deleteTree(this->_rootWindow);

    // Delete and remove window
    this->deleteWindow(this->_rootWindow);

    this->_rootWindow = NULL;

    if (this->_imageConverter) {
        delete this->_imageConverter;
        this->_imageConverter = NULL;
    }
}

void WebXDisplay::init() {
    this->_imageConverter = new WebXPNGImageConverter();

    Window rootX11Window = RootWindow(this->_x11Display, DefaultScreen(this->_x11Display));
    this->_rootWindow = this->createWindow(rootX11Window, true);
    if (this->_rootWindow) {
        this->createTree(this->_rootWindow);

        this->updateVisibleWindows();
    }

    Screen * screen = DefaultScreenOfDisplay(this->_x11Display);
    this->_screenSize = WebXSize(screen->width, screen->height);
}

WebXWindow * WebXDisplay::getWindow(Window x11Window) const {
    std::map<Window, WebXWindow *>::const_iterator it = this->_allWindows.find(x11Window);
    if (it != this->_allWindows.end()) {
        WebXWindow * window = it->second;

        return window;
    }

    return NULL;
}

WebXWindow * WebXDisplay::createWindowInTree(Window x11Window) {
    WebXWindow * window = this->createWindow(x11Window);
    if (window != NULL) {
        // Create descendents of window
        this->createTree(window);

        WebXWindow * parent = this->getParent(window);

        // Create ascendants if they aren't already in the main tree
        if (parent != NULL) {
            // printf("Added child 0x%08lx to parent 0x%08lx\n", x11Window, parent->getX11Window());
            parent->addChild(window);

        } else {
            printf("Couldn't find parent of window 0x%08lx\n", x11Window);
        }
    } 

    return window;
}

void WebXDisplay::removeWindowFromTree(Window x11Window) {
    WebXWindow * window = this->getWindow(x11Window);
    if (window != NULL) {
        // Detele descendents
        this->deleteTree(window);

        // Delete from visible windows
        tthread::lock_guard<tthread::mutex> lock(this->_visibleWindowsMutex);

        std::vector<WebXWindow *>::iterator it = std::find(this->_visibleWindows.begin(), this->_visibleWindows.end(), window);
        if (it != this->_visibleWindows.end()) {
            // Remove window
            window->disableDamage();
            this->_visibleWindows.erase(it);
        }

        // Remove window properties
        std::vector<WebXWindowProperties>::iterator it2 = this->_visibleWindowsProperties.begin();
        bool found = false;
        while (!found && it2 != this->_visibleWindowsProperties.end()) {
            if ((*it2).id == (unsigned long)window->getX11Window()) {
                found = true;

            } else {
                it2++;
            }
        }
        if (found) {
            this->_visibleWindowsProperties.erase(it2);
        }



        // Remove from parent
        WebXWindow * parent = window->getParent();
        parent->removeChild(window);

        // Delete and remove child
        this->deleteWindow(window);
    }
}

void WebXDisplay::reparentWindow(Window x11Window, Window parentX11Window) {
    WebXWindow * window = this->getWindow(x11Window);
    if (window != NULL) {
        WebXWindow * parent = window->getParent();
        if (parent != NULL) {
            parent->removeChild(window);
        }

        WebXWindow * newParent = this->getWindow(parentX11Window);
        if (newParent != NULL) {
            newParent->addChild(window);
        
        } else {
            printf("Couldn't find new parent 0x%08lx for window 0X%08lx\n", parentX11Window, x11Window);
        }
    }
}


void WebXDisplay::updateVisibleWindows() {
    // Update list of managed windows
    this->updateManagedWindows();

    this->_rootWindow->updateAttributes();

    tthread::lock_guard<tthread::mutex> lock(this->_visibleWindowsMutex);

    // Make a copy of current visible windows
    std::vector<WebXWindow *> oldVisibleWindows = this->_visibleWindows;

    // Clear current list
    this->_visibleWindows.clear();
    this->_visibleWindowsProperties.clear();

    WebXTreeDetails tree;
    if (queryTree(this->_x11Display, this->_rootWindow->getX11Window(), tree)) {
        for (unsigned int i = 0; i < tree.numberOfChildren; i++) {
            Window childX11Window = tree.children[i];
            WebXWindow * child = this->getWindow(childX11Window);
            if (child != NULL) {
                child->updateAttributes();
                if (child->isVisible(this->_rootWindow->getRectangle())) {

                    // Initialise window image
                    // if (!child->getImage()) {
                    //     this->updateImage(child);
                    // }

                    child->enableDamage();
                    this->_visibleWindows.push_back(child);

                    // Add window properties
                    this->_visibleWindowsProperties.push_back(WebXWindowProperties(child));
                } 
            }
        }
    }

    // Determine which windows are no longer visible and disable damage on them
    for (auto it = oldVisibleWindows.begin(); it != oldVisibleWindows.end(); it++) {
        WebXWindow * oldVisibleWindow = *it;
        if (find(this->_visibleWindows.begin(), this->_visibleWindows.end(), oldVisibleWindow) == this->_visibleWindows.end()) {
            oldVisibleWindow->disableDamage();
        }
    }
}

void WebXDisplay::debugTree(Window window, int indent) {

    if (window == 0) {
        window = this->_rootWindow->getX11Window();
    }

    XWindowAttributes attr;
    Status status = XGetWindowAttributes(this->_x11Display, window, &attr);
    if (attr.map_state == IsViewable) {
        // Print window
        printf("%*c", indent, ' ');
        printf("0x%08lx : @ (%d, %d), %dx%d) %d %d %d\n", window, attr.x, attr.y, attr.width, attr.height, attr.map_state, attr.depth, attr.visual ? 1 : 0);

        // Increase indent
        indent = indent + 4;

        // Get children
        WebXTreeDetails tree;
        if (queryTree(this->_x11Display, window, tree)) {
            for (unsigned int i = 0; i < tree.numberOfChildren; i++) {
                this->debugTree(tree.children[i], indent);
            }
        }
    }
}

std::shared_ptr<WebXImage> WebXDisplay::updateImage(Window x11Window) {
    tthread::lock_guard<tthread::mutex> lock(this->_visibleWindowsMutex);

    // Find visible window
    auto itWin = std::find_if(this->_visibleWindows.begin(), this->_visibleWindows.end(), 
        [&x11Window](const WebXWindow * window) {
            return window->getX11Window() == x11Window;
        });

    // Ignore damage if window is not visible
    if (itWin != this->_visibleWindows.end()) {
        WebXWindow * window = *itWin;

        return this->updateImage(window);

    } else {
        return nullptr;
    }
}

std::shared_ptr<WebXImage> WebXDisplay::getImageForVisibleWindow(Window x11Window) {
    tthread::lock_guard<tthread::mutex> lock(this->_visibleWindowsMutex);

    // Find visible window
    auto itWin = std::find_if(this->_visibleWindows.begin(), this->_visibleWindows.end(), 
        [&x11Window](const WebXWindow * window) {
            return window->getX11Window() == x11Window;
        });

    // Ignore damage if window is not visible
    if (itWin != this->_visibleWindows.end()) {
        WebXWindow * window = *itWin;
        return window->getImage();
    
    } else {
        return nullptr;
    }
}

void WebXDisplay::addDamagedWindow(Window x11Window, const WebXRectangle & damagedArea) {
    tthread::lock_guard<tthread::mutex> damageLock(this->_damagedWindowsMutex);
    tthread::lock_guard<tthread::mutex> windowsLock(this->_visibleWindowsMutex);

    // Find visible window
    auto itWin = std::find_if(this->_visibleWindows.begin(), this->_visibleWindows.end(), 
        [&x11Window](const WebXWindow * window) {
            return window->getX11Window() == x11Window;
        });

    // Ignore damage if window is not visible
    if (itWin != this->_visibleWindows.end()) {
        WebXWindow * window = *itWin;

        // See if window damage already exists
        auto it = std::find_if(this->_damagedWindows.begin(), this->_damagedWindows.end(), 
            [&x11Window](const WebXWindowDamageProperties & obj) {
                return obj.windowId == x11Window;
            });

        // Modify or add window damage
        if (it != this->_damagedWindows.end()) {
            // Add to existing damage
            WebXWindowDamageProperties & existingDamagedWindow = *it;
            existingDamagedWindow += damagedArea;
        
        } else {
            // Create new window damage
            this->_damagedWindows.push_back(WebXWindowDamageProperties(window, damagedArea));
        }
    }
}

std::vector<WebXWindowDamageProperties> WebXDisplay::getDamagedWindows(long imageUpdateUs) {
    tthread::lock_guard<tthread::mutex> lock(this->_damagedWindowsMutex);

    std::vector<WebXWindowDamageProperties> windowDamageToRepair;

    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    for (auto it = this->_damagedWindows.begin(); it != this->_damagedWindows.end();) {
        WebXWindowDamageProperties & windowDamage = *it;

        std::chrono::duration<double, std::micro> timeSinceImageUpdateUs = now - windowDamage.imageCaptureTime;
        if (timeSinceImageUpdateUs.count() > imageUpdateUs) {
            windowDamageToRepair.push_back(windowDamage);
            it = this->_damagedWindows.erase(it);

        } else {
            it++;
        }
    }

    return windowDamageToRepair;
}

uint64_t WebXDisplay::getWindowChecksum(Window x11Window) {
    tthread::lock_guard<tthread::mutex> windowsLock(this->_visibleWindowsMutex);
    
    // Find visible window
    auto itWin = std::find_if(this->_visibleWindows.begin(), this->_visibleWindows.end(), 
        [&x11Window](const WebXWindow * window) {
            return window->getX11Window() == x11Window;
        });

    // Ignore damage if window is not visible
    if (itWin != this->_visibleWindows.end()) {
        WebXWindow * window = *itWin;
        return window->getWindowChecksum();
    
    } else {
        return 0;
    }
}


void WebXDisplay::updateManagedWindows() {
    Window root = this->_rootWindow->getX11Window();
    Atom atom = XInternAtom(this->_x11Display, "_NET_CLIENT_LIST", True);
    Atom type;
    
    int form;
    unsigned long remain;
    Window * windowIds;
    unsigned long numberOfWindows;

    // Clear current list
    this->_managedWindows.clear();

    // Get list of managed IDs
    Status status = XGetWindowProperty(this->_x11Display, root, atom, 0, 1024, False, XA_WINDOW, &type, &form, &numberOfWindows, &remain, (unsigned char **)&windowIds);
    if (status == Success) {
        for (unsigned long i = 0; i < numberOfWindows; i++) {
            WebXWindow * window = this->getWindow(windowIds[i]);
            if (window == NULL) {
                window = this->createWindowInTree(windowIds[i]);
            }

            if (window != NULL) {
                WebXWindow * top = window->getTopParent();
                this->_managedWindows[top] = window;
            }
        }
    }

    XFree(windowIds);
}

WebXWindow * WebXDisplay::createWindow(Window x11Window, bool isRoot) {
    // See if already exists
    WebXWindow * window = this->getWindow(x11Window);
    if (window == NULL) {
        XWindowAttributes attr;
        Status status = XGetWindowAttributes(this->_x11Display, x11Window, &attr);
        if (status != BadWindow && attr.map_state == IsViewable) {
            window = new WebXWindow(this->_x11Display, x11Window, isRoot, attr.x, attr.y, attr.width, attr.height, (attr.map_state == IsViewable));

            this->_allWindows[x11Window] = window;
        }
    }

    return window;
}

void WebXDisplay::deleteWindow(WebXWindow * window) {
    if (window != NULL) {
        std::map<Window, WebXWindow *>::const_iterator it = this->_allWindows.find(window->getX11Window());
        if (it != this->_allWindows.end()) {
            this->_allWindows.erase(it);
        }

        delete window;
    }
}

void WebXDisplay::createTree(WebXWindow * window) {
    WebXTreeDetails tree;
    if (queryTree(this->_x11Display, window->getX11Window(), tree)) {
        for (unsigned int i = 0; i < tree.numberOfChildren; i++) {
            Window child = tree.children[i];

            // printf("Creating window with id 0x%08lx, child of 0x%08lx\n", child, window->getX11Window());
            // Create window and add window
            WebXWindow * childWindow = this->createWindow(child, false);

            if (childWindow) {
                // Add child to parent
                window->addChild(childWindow);

                // Recurse create children of window
                this->createTree(childWindow);
            }
        }

    } else {
        fprintf(stderr, "Can't query window tree for window 0x%08lx\n", window->getX11Window());
    }
}

void WebXDisplay::deleteTree(WebXWindow * window) {
    if (window) {
        std::vector<WebXWindow *> children = window->getChildren();
        for (std::vector<WebXWindow *>::iterator it = children.begin(); it != children.end(); it++) {
            WebXWindow * child = *it;

            // Delete children from child
            this->deleteTree(child);

            // Remove child from parent
            window->removeChild(child);

            // printf("Deleting window with id 0x%08lx, child of 0x%08lx\n", child->getX11Window(), window->getX11Window());
            // Delete and remove child
            this->deleteWindow(child);
        }
    }
}

WebXWindow*  WebXDisplay::getParent(WebXWindow * window) {
    WebXWindow * parent = NULL;
    WebXTreeDetails tree;
    if (queryTree(this->_x11Display, window->getX11Window(), tree)) {
        
        parent = this->getWindow(tree.parent);
        if (parent == NULL) {
            parent = this->createWindowInTree(tree.parent);
        }

    } else {
        fprintf(stderr, "Can't query window tree for window 0x%08lx\n", window->getX11Window());
    }

    return parent;
}

std::shared_ptr<WebXImage> WebXDisplay::updateImage(WebXWindow * window) const {
    WebXRectangle subWindowRectangle = window->getRectangle();
    WebXWindow * managedWindow = this->getManagedWindow(window);
    if (managedWindow != NULL) {
        subWindowRectangle = managedWindow->getSubWindowRectangle();
        return window->updateImage(&subWindowRectangle, this->_imageConverter);

    } else {
        return window->updateImage(NULL, this->_imageConverter);
    }
}
