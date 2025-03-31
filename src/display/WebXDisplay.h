#ifndef WEBX_DISPLAY_H
#define WEBX_DISPLAY_H

#include <X11/Xlib.h>
#include <map>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include "WebXWindowProperties.h"
#include <models/WebXQuality.h>
#include <models/WebXSize.h>

class WebXWindow;
class WebXImageConverter;
class WebXMouse;
class WebXKeyboard;

/**
 * @class WebXDisplay
 * @brief Manages the X11 display and its associated windows.
 * 
 * This class provides methods to initialize the display, manage windows, retrieve
 * window properties, and handle mouse and keyboard interactions.
 */
class WebXDisplay {
public:
    /**
     * @brief Constructs a WebXDisplay instance.
     * @param display Pointer to the X11 display.
     */
    WebXDisplay(Display * display);

    /**
     * @brief Destructor.
     */
    virtual ~WebXDisplay();

    /**
     * @brief Initializes the display.
     */
    void init();

    /**
     * @brief Retrieves the screen size of the display.
     * @return Reference to the screen size.
     */
    const WebXSize & getScreenSize() const {
        return this->_screenSize;
    }

    /**
     * @brief Retrieves a window by its X11 window ID.
     * @param window X11 window ID.
     * @return Pointer to the WebXWindow instance, or nullptr if not found.
     */
    WebXWindow * getWindow(Window window) const;

    /**
     * @brief Retrieves the visible window by its X11 window ID.
     * @param window X11 window ID.
     * @return Pointer to the visible WebXWindow instance, or nullptr if not found.
     */
    WebXWindow * getVisibleWindow(Window window);

    /**
     * @brief Creates a window in the window tree.
     * @param x11Window X11 window ID.
     * @return Pointer to the created WebXWindow instance.
     */
    WebXWindow * createWindowInTree(Window x11Window);

    /**
     * @brief Removes a window from the window tree.
     * @param x11Window X11 window ID.
     */
    void removeWindowFromTree(Window x11Window);

    /**
     * @brief Reparents a window to a new parent in the window tree.
     * @param x11Window X11 window ID of the window to reparent.
     * @param parentX11Window X11 window ID of the new parent.
     */
    void reparentWindow(Window x11Window, Window parentX11Window);

    /**
     * @brief Retrieves the visibility properties of all visible windows.
     * @return Vector of pointers to the visibility properties of visible windows.
     */
    const std::vector<const WebXWindowVisibility *> getWindowVisiblities() {
        const std::lock_guard<std::mutex> lock(this->_visibleWindowsMutex);
        std::vector<const WebXWindowVisibility *> windowVisibilities;

        std::transform(this->_visibleWindows.begin(), this->_visibleWindows.end(), std::back_inserter(windowVisibilities), [](WebXWindow * window) { 
            return &window->getVisibility();
        });

        return windowVisibilities;
    }

    /**
     * @brief Retrieves the properties of all visible windows.
     * @return Vector of WebXWindowProperties for visible windows.
     */
    const std::vector<WebXWindowProperties> getVisibleWindowsProperties() {
        const std::lock_guard<std::mutex> lock(this->_visibleWindowsMutex);
        std::vector<WebXWindowProperties> visibleWindowsProperties;

        std::transform(this->_visibleWindows.begin(), this->_visibleWindows.end(), std::back_inserter(visibleWindowsProperties), [](WebXWindow * window) { 
            return WebXWindowProperties(window);
        });

        return visibleWindowsProperties;
    }

    /**
     * @brief Retrieves the root window of the display.
     * @return Pointer to the root WebXWindow instance.
     */
    WebXWindow * getRootWindow() const {
        return this->_rootWindow;
    }

    /**
     * @brief Updates the list of visible windows.
     */
    void updateVisibleWindows();

    /**
     * @brief Prints the window tree for debugging purposes.
     * @param root X11 window ID of the root window (default is 0).
     * @param indent Indentation level for the debug output (default is 0).
     */
    void debugTree(Window root = 0, int indent = 0);

    /**
     * @brief Retrieves an image of a window.
     * @param x11Window X11 window ID.
     * @param quality Requested quality of the image.
     * @param imageRectangle Optional rectangle representing the area to capture.
     * @return Shared pointer to the captured image.
     */
    std::shared_ptr<WebXImage> getImage(Window x11Window, const WebXQuality & quality, const WebXRectangle * imageRectangle = nullptr);

    /**
     * @brief Retrieves the mouse instance associated with the display.
     * @return Pointer to the WebXMouse instance.
     */
    WebXMouse * getMouse() const {
        return this->_mouse;
    }

    /**
     * @brief Updates the mouse cursor position.
     */
    void updateMouseCursor();

    /**
     * @brief Sends a mouse instruction to the client.
     * @param x X-coordinate of the mouse position.
     * @param y Y-coordinate of the mouse position.
     * @param buttonMask Button mask representing mouse button states.
     */
    void sendClientMouseInstruction(int x, int y, unsigned int buttonMask);

    /**
     * @brief Sends a keyboard event to the client.
     * @param keysym Key symbol of the key.
     * @param pressed True if the key is pressed, false if released.
     */
    void sendKeyboard(int keysym, bool pressed);

    /**
     * @brief Loads a keyboard layout.
     * @param layout Keyboard layout string.
     */
    void loadKeyboardLayout(const std::string & layout);
    
private:
    /**
     * @struct WebXTreeDetails
     * @brief Represents the details of a window tree in the X11 display.
     * 
     * This structure holds information about the root, parent, and children of a window,
     * as well as the number of children.
     */
    struct WebXTreeDetails {
        /**
         * @brief Default constructor. Initializes members to default values.
         */
        WebXTreeDetails() :
            root(0),
            parent(0),
            children(NULL),
            numberOfChildren(0) {
        }

        /**
         * @brief Destructor. Frees the memory allocated for child windows.
         */
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

   /**
     * @brief Queries the X11 window tree for a given window.
     * @param display Pointer to the X11 display.
     * @param window X11 window ID.
     * @param tree Reference to the WebXTreeDetails structure to populate.
     * @return True if the query was successful, false otherwise.
     */
    bool queryTree(Display * display, Window window, WebXTreeDetails & tree) const {
        return XQueryTree(display, window, &tree.root, &tree.parent, &tree.children, &tree.numberOfChildren);
    }

private:
    /**
     * @brief Creates a WebXWindow instance.
     * @param x11Window X11 window ID.
     * @param isRoot True if the window is the root window, false otherwise.
     * @return Pointer to the created WebXWindow instance.
     */
    WebXWindow * createWindow(Window x11Window, bool isRoot = false);

    /**
     * @brief Deletes a WebXWindow instance.
     * @param window Pointer to the WebXWindow instance to delete.
     */
    void deleteWindow(WebXWindow * window);

    /**
     * @brief Creates a tree structure of windows starting from the root.
     * @param root Pointer to the root WebXWindow instance.
     */
    void createTree(WebXWindow * root);

    /**
     * @brief Deletes a tree structure of windows starting from the root.
     * @param root Pointer to the root WebXWindow instance.
     */
    void deleteTree(WebXWindow * root);

    /**
     * @brief Retrieves the parent window of a given window.
     * @param window Pointer to the WebXWindow instance.
     * @return Pointer to the parent WebXWindow instance.
     */
    WebXWindow * getParent(WebXWindow * window);

    /**
     * @brief Updates the coverage information of all visible windows.
     */
    void updateWindowCoverage();

private:
    Display * _x11Display;

    WebXSize _screenSize;

    WebXWindow * _rootWindow;
    std::map<Window, WebXWindow *> _allWindows;

    std::vector<WebXWindow *> _visibleWindows;
    std::mutex _visibleWindowsMutex;

    WebXImageConverter * _imageConverter;

    WebXMouse * _mouse;
    WebXKeyboard * _keyboard;
};


#endif /* WEBX_DISPLAY_H */