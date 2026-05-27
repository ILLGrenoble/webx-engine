#ifndef WEBX_X11_DISPLAY_H
#define WEBX_X11_DISPLAY_H

#include <X11/Xlib.h>
#include <map>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <display/WebXDisplay.h>
#include <display/WebXWindowProperties.h>
#include <models/WebXQuality.h>
#include <models/WebXSize.h>

class WebXWindow;
class WebXImageConverter;
class WebXx11Mouse;
class WebXKeyboard;
class WebXRandR;
class WebXRandREvent;
class WebXImage;
class WebXWindowVisibility;
class WebXRectangle;

/**
 * @class WebXx11Display
 * @brief Manages the X11 display and its associated windows.
 * 
 * This class provides methods to initialize the display, manage windows, retrieve
 * window properties, and handle mouse and keyboard interactions.
 */
class WebXx11Display : public WebXDisplay {
public:
    /**
     * @brief Constructs a WebXx11Display instance.
     * @param display Pointer to the X11 display.
     */
    WebXx11Display(Display * display);

    /**
     * @brief Destructor.
     */
    virtual ~WebXx11Display();

    /**
     * @brief Initializes the display.
     * @param rootWindowMode Optional root-window mode (individual windows ignored, only damage events on root window used)
     */
    void init(bool rootWindowMode = false);

    /**
     * @brief Retrieves the screen size of the display.
     * @return Reference to the screen size.
     */
    virtual const WebXSize getScreenSize() const;

    /**
     * @brief Retrieves a window by its X11 window ID.
     * @param window X11 window ID.
     * @return Pointer to the WebXWindow instance, or nullptr if not found.
     */
    WebXWindow * getWindow(Window window) const;

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
    virtual const std::vector<const WebXWindowVisibility *> getWindowVisiblities();

    /**
     * @brief Retrieves the properties of all visible windows.
     * @return Vector of WebXWindowProperties for visible windows.
     */
    virtual const std::vector<WebXWindowProperties> getVisibleWindowsProperties();

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
    virtual std::shared_ptr<WebXImage> getImage(Window x11Window, const WebXQuality & quality, const WebXRectangle * imageRectangle = nullptr);

    /**
     * @brief Retrieves the shape mask image of a window.
     * @param x11Window X11 window ID.
     * @return Shared pointer to the captured image.
     */
    virtual std::shared_ptr<WebXImage> getWindowShapeMask(Window x11Window);

    /**
     * @brief Specifies that the shape for the specified window needs to be recalculated (typically after a shape-related event)
     * @param x11Window X11 window ID.
     */
    void updateWindowShape(Window x11Window);

    /**
     * @brief Retrieves the mouse instance associated with the display.
     * @return Pointer to the WebXMouse instance.
     */
    virtual WebXMouse * getMouse() const;

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
    virtual void sendClientMouseInstruction(int x, int y, unsigned int buttonMask);

    /**
     * @brief Sends a keyboard event to the client.
     * @param keysym Key symbol of the key.
     * @param pressed True if the key is pressed, false if released.
     */
    virtual void sendKeyboard(int keysym, bool pressed);

    /**
     * @brief Loads a keyboard layout.
     * @param layout Keyboard layout or name string.
     * @return true if the keyboard was loaded successfully
     */
    virtual bool loadKeyboardLayout(const std::string & layoutOrName);

    /**
     * @brief Returns the current keyboard layout name.
     * @return the current keyboard layout name.
     */
    virtual std::string getKeyboardLayoutName() const;
    
    /**
     * @brief Determines if the screen can be resized
     * @return true if the screen can be resized
     */
    virtual bool canResizeScreen() const;
    
    /**
     * @brief Resizes the screen
     * @param width The requested screen width.
     * @param height The requested screen height.
     */
    virtual void resizeScreen(unsigned int width, unsigned int height);
    
    /**
     * @brief Determines if the randr event is valid
     * @param event The randr event.
     * @return true if the event is valid
     */
    bool isValidRandREvent(const WebXRandREvent & event) const;

    /**
     * Generic method to allow retrieval of visible WebXWindow and function to be called on it (if the window is found/visible).
     * Assures that the function is called within the scope of the _visibleWindowsMutex
     * @param window X11 window ID.
     * @param visibleWindowCallable The function to be called on the visible window
     */
    void callIfWindowVisible(Window x11Window, std::function<void(WebXWindow * window)> visibleWindowCallable);

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
     * @return Pointer to the created WebXWindow instance.
     */
    WebXWindow * createWindow(Window x11Window);

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

    WebXWindow * _rootWindow;
    std::map<Window, WebXWindow *> _allWindows;

    std::vector<WebXWindow *> _visibleWindows;
    std::mutex _visibleWindowsMutex;

    WebXImageConverter * _imageConverter;

    WebXx11Mouse * _mouse;
    WebXKeyboard * _keyboard;
    WebXRandR * _randr;
};


#endif /* WEBX_X11_DISPLAY_H */