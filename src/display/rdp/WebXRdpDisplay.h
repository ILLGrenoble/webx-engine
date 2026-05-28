#ifndef WEBX_RDP_DISPLAY_H
#define WEBX_RDP_DISPLAY_H

#include <display/WebXDisplay.h>
#include <models/WebXQuality.h>
#include <models/WebXSize.h>
#include <models/WebXWindowVisibility.h>
#include <image/WebXPixelBuffer.h>

class WebXRdpClient;
class WebXRdpMouse;
class WebXImageConverter;

class WebXRdpDisplay : public WebXDisplay {
public:
    /**
     * @brief Constructs a WebXRdpDisplay instance.
     */
    WebXRdpDisplay(WebXRdpClient * rdpClient);

    /**
     * @brief Destructor.
     */
    virtual ~WebXRdpDisplay();

    void init();    


    void updateFramebuffer(const std::vector<WebXRectangle> & invalidRectangles, const WebXPixelBuffer & framebuffer);

    /**
     * @brief Retrieves the screen size of the display.
     * @return Reference to the screen size.
     */
    virtual const WebXSize getScreenSize() const;

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
     * @brief Retrieves an image of a window.
     * @param windowId the window ID.
     * @param quality Requested quality of the image.
     * @param imageRectangle Optional rectangle representing the area to capture.
     * @return Shared pointer to the captured image.
     */
    virtual std::shared_ptr<WebXImage> getImage(unsigned long windowId, const WebXQuality & quality, const WebXRectangle * imageRectangle = nullptr);

    /**
     * @brief Retrieves the shape mask image of a window.
     * @param windowId the window ID.
     * @return Shared pointer to the captured image.
     */
    virtual std::shared_ptr<WebXImage> getWindowShapeMask(unsigned long windowId);

    /**
     * @brief Retrieves the mouse instance associated with the display.
     * @return Pointer to the WebXMouse instance.
     */
    virtual WebXMouse * getMouse() const;

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
    
private:
    WebXWindowVisibility getWindowVisibility() const;
    WebXWindowProperties getWindowProperties() const;

private:
    WebXRdpClient * _rdpClient;
    WebXWindowVisibility _windowVisibility;

    WebXImageConverter * _imageConverter;

    WebXRdpMouse * _mouse;

    std::unique_ptr<WebXPixelBuffer> _framebuffer;
};


#endif /* WEBX_RDP_DISPLAY_H */