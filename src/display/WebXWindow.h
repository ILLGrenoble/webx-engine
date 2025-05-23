#ifndef WEBX_WINDOW_H
#define WEBX_WINDOW_H

#include <X11/Xlib.h>
#include <X11/extensions/Xdamage.h>
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <mutex>
#include <image/WebXImageConverter.h>
#include <image/WebXImage.h>
#include <models/WebXQuality.h>
#include <models/WebXRectangle.h>
#include <models/WebXWindowCoverage.h>
#include <models/WebXWindowVisibility.h>
#include "WebXWindowShape.h"

class WebXWindowShape;

/**
 * @class WebXWindow
 * @brief Represents an X11 window and its associated properties and behaviors.
 * 
 * This class provides methods to interact with and manage an X11 window, including
 * retrieving its attributes, managing its children, and handling visibility and damage.
 */
class WebXWindow {

public:
    /**
     * @brief Constructs a WebXWindow instance.
     * @param display Pointer to the X11 display.
     * @param window X11 window handle.
     * @param isRoot Indicates if the window is the root window.
     * @param x X-coordinate of the window.
     * @param y Y-coordinate of the window.
     * @param width Width of the window.
     * @param height Height of the window.
     * @param isViewable Indicates if the window is viewable.
     */
    WebXWindow(Display * display, Window window, bool isRoot, int x, int y, int width, int height, bool isViewable);

    /**
     * @brief Destructor.
     */
    virtual ~WebXWindow();

    /**
     * @brief Retrieves the X11 window handle.
     * @return X11 window handle.
     */
    Window getX11Window() const {
        return this->_x11Window;
    } 

    /**
     * @brief Checks if the window is the root window.
     * @return True if the window is the root window, false otherwise.
     */
    bool isRoot() const {
        return this->_isRoot;
    }

    /**
     * @brief Retrieves the rectangle representing the window's position and size.
     * @return Rectangle representing the window's position and size.
     */
    const WebXRectangle & getRectangle() const {
        return this->_visibility.getRectangle();
    }

    /**
     * @brief Updates the window's attributes.
     * @return Status of the update operation.
     */
    Status updateAttributes();

    /**
     * @brief Prints information about the window.
     */
    void printInfo() const;

    /**
     * @brief Retrieves an image of the window.
     * @param imageRectangle Rectangle representing the area of the window to capture.
     * @param imageConverter Pointer to the image converter.
     * @param requestedQuality Requested quality of the image.
     * @return Shared pointer to the captured image.
     */
    std::shared_ptr<WebXImage> getImage(const WebXRectangle * imageRectangle, WebXImageConverter * imageConverter, const WebXQuality & requestedQuality);

    /**
     * Updates the WindowShape: takes into account that the window may not be rectangular
     * @param imageConverter Pointer to the image converter.
     * @param quality Quality settings for the shape.
     */
    void updateShape(WebXImageConverter * imageConverter, const WebXQuality & quality) {
        this->_shape.update(this->getRectangle().size().width(), this->getRectangle().size().height(), imageConverter, quality);
    }

    /**
     * @brief Returns true if the window has a shape (non-rectangular).
     */
    bool hasShape() const {
        return this->_shape.isActive();
    }

    /**
     * @brief Retrieves the shape mask of the window.
     * @return Shared pointer to the shape mask image.
     */
    std::shared_ptr<WebXImage> getShapeMask() const {
        return this->_shape.getShapeMask();
    }

    /**
     * @brief Retrieves the parent window.
     * @return Pointer to the parent window.
     */
    WebXWindow * getParent() const {
        return this->_parent;
    }

    /**
     * @brief Sets the parent window.
     * @param parent Pointer to the parent window.
     */
    void setParent(WebXWindow * parent) {
        this->_parent = parent;
    }

    /**
     * @brief Adds a child window.
     * @param child Pointer to the child window.
     */
    void addChild(WebXWindow * child);

    /**
     * @brief Removes a child window.
     * @param child Pointer to the child window.
     */
    void removeChild(WebXWindow * child);
    
    /**
     * @brief Retrieves the list of child windows.
     * @return Vector of pointers to the child windows.
     */
    const std::vector<WebXWindow *> & getChildren() const {
        return this->_children;
    }

    /**
     * @brief Checks if the window is visible within the given viewport.
     * @param viewport Size of the viewport.
     * @return True if the window is visible, false otherwise.
     */
    bool isVisible(const WebXSize & viewport) const {
        return this->_visibility.isVisible(viewport);
    }

    /**
     * @brief Sets the viewable state of the window.
     * @param isViewable True if the window is viewable, false otherwise.
     */
    void setIsViewable(bool isViewable) {
        this->_visibility.setViewable(isViewable);
    }

    /**
     * @brief Checks if the window is viewable.
     * @return True if the window is viewable, false otherwise.
     */
    bool isViewable() const {
        return this->_visibility.isViewable();
    }

    /**
     * @brief Sets the coverage of the window.
     * @param coverage Coverage of the window.
     */
    void setCoverage(const WebXWindowCoverage & coverage) {
        this->_visibility.setCoverage(coverage);
    }

    /**
     * @brief Retrieves the visibility properties of the window.
     * @return Visibility properties of the window.
     */
    const WebXWindowVisibility & getVisibility() const {
        return this->_visibility;
    }
    
    /**
     * @brief Enables damage tracking for the window.
     */
    void enableDamage();

    /**
     * @brief Disables damage tracking for the window.
     */
    void disableDamage();

private:
    Display * _display;
    Window _x11Window;
    Damage _damage;
    bool _isRoot;

    WebXWindow * _parent;
    std::vector<WebXWindow *> _children;

    WebXWindowVisibility _visibility;
    WebXWindowShape _shape;

    std::mutex _damageMutex;
};


#endif /* WEBX_WINDOW_H */