#ifndef WEBX_WINDOW_PROPERTIES_H
#define WEBX_WINDOW_PROPERTIES_H

#include "WebXWindow.h"

/**
 * @class WebXWindowProperties
 * @brief Represents the properties of a WebXWindow, such as its position, size, and ID.
 * 
 * This class is used to encapsulate the properties of a WebXWindow, including its
 * X11 window ID, position (x, y), and dimensions (width, height). It provides
 * constructors for initialization and an assignment operator for copying properties.
 */
class WebXWindowProperties {
public:
    /**
     * @brief Constructs WebXWindowProperties from a WebXWindow instance.
     * @param window Pointer to the WebXWindow instance.
     */
    WebXWindowProperties(WebXWindow * window) :
        id((unsigned long)window->getX11Window()),
        x(window->getRectangle().x()),
        y(window->getRectangle().y()),
        width(window->getRectangle().size().width()),
        height(window->getRectangle().size().height()),
        hasShape(window->hasShape()) {
    }

    /**
     * @brief Copy constructor.
     * @param window Reference to another WebXWindowProperties instance.
     */
    WebXWindowProperties(const WebXWindowProperties & window) :
        id(window.id),
        x(window.x),
        y(window.y),
        width(window.width),
        height(window.height),
        hasShape(window.hasShape) {
    }

    /**
     * @brief Destructor.
     */
    virtual ~WebXWindowProperties() {}

    /**
     * @brief X11 window ID.
     */
    const unsigned long id;

    /**
     * @brief X-coordinate of the window's position.
     */
    const int x;

    /**
     * @brief Y-coordinate of the window's position.
     */
    const int y;

    /**
     * @brief Width of the window.
     */
    const int width;

    /**
     * @brief Height of the window.
     */
    const int height;

    /**
     * @brief Indicates if the window has a shape (non-rectangular).
     */
    const bool hasShape;
};

#endif /* WEBX_WINDOW_PROPERTIES */