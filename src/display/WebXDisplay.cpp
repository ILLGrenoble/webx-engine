#include "WebXDisplay.h"
#include "WebXWindow.h"
#include <image/WebXJPGImageConverter.h>
#include <algorithm>
#include <X11/Xatom.h>
#include <spdlog/spdlog.h>
#include <input/WebXMouse.h>
#include <input/WebXKeyboard.h>
#include <utils/WebXQualityHelper.h>

WebXDisplay::WebXDisplay(Display * display) :
    _x11Display(display),
    _rootWindow(NULL),
    _imageConverter(new WebXJPGImageConverter()),
    _mouse(NULL) {

}

WebXDisplay::~WebXDisplay() {
    this->deleteTree(this->_rootWindow);

    // Delete and remove window
    this->deleteWindow(this->_rootWindow);

    this->_rootWindow = NULL;

    delete this->_imageConverter;
    this->_imageConverter = NULL;

    if (this->_mouse) {
        delete this->_mouse;
        this->_mouse = NULL;
    }

    if (this->_keyboard) {
        delete this->_keyboard;
        this->_keyboard = NULL;
    }

}

void WebXDisplay::init() {
    Window rootX11Window = RootWindow(this->_x11Display, DefaultScreen(this->_x11Display));
    Screen * screen = DefaultScreenOfDisplay(this->_x11Display);
    this->_screenSize = WebXSize(screen->width, screen->height);
    this->_mouse = new WebXMouse(this->_x11Display, rootX11Window);
    this->_keyboard = new WebXKeyboard(this->_x11Display);
    this->_keyboard->init();

    this->_rootWindow = this->createWindow(rootX11Window, true);
    if (this->_rootWindow) {
        this->createTree(this->_rootWindow);

        this->updateVisibleWindows();
    }
}

WebXWindow * WebXDisplay::getWindow(Window x11Window) const {
    std::map<Window, WebXWindow *>::const_iterator it = this->_allWindows.find(x11Window);
    if (it != this->_allWindows.end()) {
        WebXWindow * window = it->second;

        return window;
    }

    return NULL;
}

WebXWindow * WebXDisplay::getVisibleWindow(Window x11Window) {
    std::lock_guard<std::mutex> windowsLock(this->_visibleWindowsMutex);
    
    // Find visible window
    auto itWin = std::find_if(this->_visibleWindows.begin(), this->_visibleWindows.end(), 
        [&x11Window](const WebXWindow * window) {
            return window->getX11Window() == x11Window;
        });

    // Return 0 if window is not visible
    if (itWin != this->_visibleWindows.end()) {
        WebXWindow * window = *itWin;
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
            spdlog::trace("Added child 0x{:01x} to parent 0x{:01x}", x11Window, parent->getX11Window());
            parent->addChild(window);

        } else {
            spdlog::error("Couldn't find parent of window 0x{:01x}", x11Window);
        }
    } 

    return window;
}

/**
 * Window has been removed from the X11 tree (the window is no longer valid and will generate errors if X calls are made on it) 
 */
void WebXDisplay::removeWindowFromTree(Window x11Window) {
    WebXWindow * window = this->getWindow(x11Window);
    if (window != NULL) {
        // Detele descendents
        this->deleteTree(window);

        // Delete from visible windows
        std::lock_guard<std::mutex> lock(this->_visibleWindowsMutex);

        // Set manually the isViewable attribute (cannot call XGetWindowAttributes as window is no longer valid)
        window->setIsViewable(false);

        std::vector<WebXWindow *>::iterator it = std::find(this->_visibleWindows.begin(), this->_visibleWindows.end(), window);
        if (it != this->_visibleWindows.end()) {
            // Remove window
            this->_visibleWindows.erase(it);
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

    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

    this->_rootWindow->updateAttributes();

    std::lock_guard<std::mutex> lock(this->_visibleWindowsMutex);

    // Make a copy of current visible windows
    std::vector<WebXWindow *> oldVisibleWindows = this->_visibleWindows;
    std::vector<Window> allX11Windows;

    // Clear current list
    this->_visibleWindows.clear();

    WebXTreeDetails tree;
    if (queryTree(this->_x11Display, this->_rootWindow->getX11Window(), tree)) {
        for (unsigned int i = 0; i < tree.numberOfChildren; i++) {
            Window childX11Window = tree.children[i];
            
            allX11Windows.push_back(childX11Window);
            
            WebXWindow * child = this->getWindow(childX11Window);
            if (child != NULL) {
                Status status = child->updateAttributes();
                if (status && child->isVisible(this->_rootWindow->getRectangle().size)) {

                    child->enableDamage();
                    this->_visibleWindows.push_back(child);
                } 
            }
        }
    }

    // Determine which windows are no longer visible and disable damage on them
    for (auto it = oldVisibleWindows.begin(); it != oldVisibleWindows.end(); it++) {
        WebXWindow * oldVisibleWindow = *it;
        if (find(this->_visibleWindows.begin(), this->_visibleWindows.end(), oldVisibleWindow) == this->_visibleWindows.end()) {
            // Check that child still exists
            if (find(allX11Windows.begin(), allX11Windows.end(), oldVisibleWindow->getX11Window()) != allX11Windows.end()) {
                oldVisibleWindow->disableDamage();
            
            } else {
                oldVisibleWindow->setIsViewable(false);
            }
        }
    }

    std::chrono::high_resolution_clock::time_point updateTime = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> updateDuration = updateTime - start;

    this->updateWindowQualities();

    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> coverageDuration = end - updateTime;
    std::chrono::duration<double, std::milli> duration = end - start;

    spdlog::trace("Updated visible windows: {:d} windows in {:.2f}ms (update = {:.2f}ms, coverage = {:.2f}ms)", this->_visibleWindows.size(), duration.count(), updateDuration.count(), coverageDuration.count());
}

void WebXDisplay::updateWindowQualities() {

    const WebXMouseState * mouseState = this->getMouse()->getState();

    // Calculate visible areas of each visible window
    for (auto it = _visibleWindows.begin(); it != _visibleWindows.end(); it++) {
        WebXWindow * window = *it;

        auto it2 = it;
        it2++;
        std::vector<WebXRectangle> coveringRectangles;
        std::transform(it2, this->_visibleWindows.end(), std::back_inserter(coveringRectangles), [](WebXWindow * window) { return window->getRectangle(); });

        WebXRectangle::WebXRectCoverage overlap = window->getRectangle().overlapCalc(coveringRectangles, mouseState->getX(), mouseState->getY());
        window->setCoverage(overlap.coverage);

        // quality dependent on coverage only
        // const WebXQuality & quality = webx_quality_for_image_coverage(overlap.coverage);

        // quality dependent on coverage, but mouse over visible part of window improves quality
        const WebXQuality & quality = overlap.mouseOver ? webx_quality_for_index(WebXQuality::MAX_QUALITY_INDEX) : webx_quality_for_image_coverage(overlap.coverage);

        window->setQuality(quality);
    }
}

void WebXDisplay::debugTree(Window window, int indent) {

    if (window == 0) {
        window = this->_rootWindow->getX11Window();
    }

    XWindowAttributes attr;
    Status status = XGetWindowAttributes(this->_x11Display, window, &attr);
    if (attr.map_state == IsViewable && attr.c_class == InputOutput) {
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

std::shared_ptr<WebXImage> WebXDisplay::getImage(Window x11Window, float quality , WebXRectangle * imageRectangle) {
    std::lock_guard<std::mutex> lock(this->_visibleWindowsMutex);

    // Find visible window
    auto itWin = std::find_if(this->_visibleWindows.begin(), this->_visibleWindows.end(), 
        [&x11Window](const WebXWindow * window) {
            return window->getX11Window() == x11Window;
        });

    // Ignore damage if window is not visible
    if (itWin != this->_visibleWindows.end()) {
        WebXWindow * window = *itWin;
        return window->getImage(imageRectangle, this->_imageConverter, quality);

    } else {
        return nullptr;
    }
}

void WebXDisplay::addDamagedWindow(Window x11Window, const WebXRectangle & damagedArea, bool fullWindowRefresh) {
    std::lock_guard<std::mutex> damageLock(this->_damagedWindowsMutex);
    std::lock_guard<std::mutex> windowsLock(this->_visibleWindowsMutex);

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
            if (fullWindowRefresh) {
                // Delete previous window damage (window size has changed)
                this->_damagedWindows.erase(it);

                // Create new window damage
                this->_damagedWindows.push_back(WebXWindowDamageProperties(window, damagedArea, fullWindowRefresh));
            
            } else {
                // Add to existing damage
                WebXWindowDamageProperties & existingDamagedWindow = *it;
                existingDamagedWindow += damagedArea;
            }

        } else {
            // Create new window damage
            this->_damagedWindows.push_back(WebXWindowDamageProperties(window, damagedArea, fullWindowRefresh));
        }
    }
}

std::vector<WebXWindowDamageProperties> WebXDisplay::getDamagedWindows(const WebXQuality & quality) {
    std::lock_guard<std::mutex> lock(this->_damagedWindowsMutex);

    std::vector<WebXWindowDamageProperties> windowDamageToRepair;

    std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
    for (auto it = this->_damagedWindows.begin(); it != this->_damagedWindows.end();) {
        WebXWindowDamageProperties & windowDamage = *it;

        // Verify that the window still exists
        WebXWindow * window = this->getWindow(windowDamage.windowId);
        float imageUpdateTimeUs = quality.imageUpdateTimeUs;
        if (window != NULL) {
            // Modify quality to be poorest between requested and calculated
            if (window->getQuality().index < quality.index) {
                imageUpdateTimeUs = window->getQuality().imageUpdateTimeUs;
            }

            std::chrono::duration<double, std::micro> timeSinceImageUpdateUs = now - windowDamage.imageCaptureTime;
            if (timeSinceImageUpdateUs.count() > imageUpdateTimeUs) {
                windowDamageToRepair.push_back(windowDamage);
                it = this->_damagedWindows.erase(it);
    
            } else {
                it++;
            }

        } else {
            it = this->_damagedWindows.erase(it);
        }

    }

    return windowDamageToRepair;
}

void WebXDisplay::updateMouseCursor() {
    this->_mouse->updateCursor();
}

void WebXDisplay::sendClientMouseInstruction(int x, int y, unsigned int buttonMask) {
    spdlog::trace("Sending mouse instruction x={}, y={}, buttonMask={}", x, y, buttonMask);
    this->_mouse->sendClientInstruction(x, y, buttonMask);
    this->updateWindowQualities();
}

void WebXDisplay::sendKeyboard(int keysym, bool pressed) {
    spdlog::trace("Sending keyboard instruction key={}, pressed={}", keysym, pressed);
    this->_keyboard->handleKeySym(keysym, pressed, true);
}

void WebXDisplay::loadKeyboardLayout(const std::string & layout) {
    if (!layout.empty()) {
        if (this->_keyboard->loadKeyboardLayout(layout)) {
            spdlog::info("Loaded '{:s}' keyboard layout", layout);
    
        } else {
            spdlog::error("Failed to load '{:s}' keyboard layout", layout);
        }
    }
}

WebXWindow * WebXDisplay::createWindow(Window x11Window, bool isRoot) {
    // See if already exists
    WebXWindow * window = this->getWindow(x11Window);
    if (window == NULL) {
        XWindowAttributes attr;
        Status status = XGetWindowAttributes(this->_x11Display, x11Window, &attr);
        if (status != BadWindow && attr.map_state == IsViewable && attr.c_class == InputOutput) {
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
        spdlog::error("Can't query window tree for window 0x{:08x}", window->getX11Window());
    }

    return parent;
}



