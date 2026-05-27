#ifndef WEBX_DISPLAY_H
#define WEBX_DISPLAY_H

#include <display/WebXWindowProperties.h>
#include <models/WebXQuality.h>
#include <models/WebXSize.h>

class WebXMouse;
class WebXImage;
class WebXWindowVisibility;
class WebXRectangle;

class WebXDisplay {
public:
    /**
     * @brief Constructs a WebXDisplay instance.
     */
    WebXDisplay() {}

    /**
     * @brief Destructor.
     */
    virtual ~WebXDisplay() {}

    /**
     * @brief Retrieves the screen size of the display.
     * @return Reference to the screen size.
     */
    virtual const WebXSize getScreenSize() const = 0;

    /**
     * @brief Retrieves the visibility properties of all visible windows.
     * @return Vector of pointers to the visibility properties of visible windows.
     */
    virtual const std::vector<const WebXWindowVisibility *> getWindowVisiblities() = 0;

    /**
     * @brief Retrieves the properties of all visible windows.
     * @return Vector of WebXWindowProperties for visible windows.
     */
    virtual const std::vector<WebXWindowProperties> getVisibleWindowsProperties() = 0;

    /**
     * @brief Retrieves an image of a window.
     * @param windowId the window ID.
     * @param quality Requested quality of the image.
     * @param imageRectangle Optional rectangle representing the area to capture.
     * @return Shared pointer to the captured image.
     */
    virtual std::shared_ptr<WebXImage> getImage(unsigned long windowId, const WebXQuality & quality, const WebXRectangle * imageRectangle = nullptr) = 0;

    /**
     * @brief Retrieves the shape mask image of a window.
     * @param windowId the window ID.
     * @return Shared pointer to the captured image.
     */
    virtual std::shared_ptr<WebXImage> getWindowShapeMask(unsigned long windowId) = 0;

    /**
     * @brief Retrieves the mouse instance associated with the display.
     * @return Pointer to the WebXMouse instance.
     */
    virtual WebXMouse * getMouse() const = 0;

    /**
     * @brief Sends a mouse instruction to the client.
     * @param x X-coordinate of the mouse position.
     * @param y Y-coordinate of the mouse position.
     * @param buttonMask Button mask representing mouse button states.
     */
    virtual void sendClientMouseInstruction(int x, int y, unsigned int buttonMask) = 0;

    /**
     * @brief Sends a keyboard event to the client.
     * @param keysym Key symbol of the key.
     * @param pressed True if the key is pressed, false if released.
     */
    virtual void sendKeyboard(int keysym, bool pressed) = 0;

    /**
     * @brief Loads a keyboard layout.
     * @param layout Keyboard layout or name string.
     * @return true if the keyboard was loaded successfully
     */
    virtual bool loadKeyboardLayout(const std::string & layoutOrName) = 0;

    /**
     * @brief Returns the current keyboard layout name.
     * @return the current keyboard layout name.
     */
    virtual std::string getKeyboardLayoutName() const = 0;
    
    /**
     * @brief Determines if the screen can be resized
     * @return true if the screen can be resized
     */
    virtual bool canResizeScreen() const = 0;
    
    /**
     * @brief Resizes the screen
     * @param width The requested screen width.
     * @param height The requested screen height.
     */
    virtual void resizeScreen(unsigned int width, unsigned int height) = 0;
    
};


#endif /* WEBX_DISPLAY_H */