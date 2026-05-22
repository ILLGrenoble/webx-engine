#ifndef WEBX_WINDOW_PROPERTIES_H
#define WEBX_WINDOW_PROPERTIES_H

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
     * @param id the window Id
     * @param x the window x coordinate
     * @param y the window y coordinate
     * @param width the window width
     * @param height the window height
     * @param hasShape specified if the window has a shape mask
     */
    WebXWindowProperties(unsigned long id, int x, int y, int width, int height, bool hasShape) :
        id(id),
        x(x),
        y(y),
        width(width),
        height(height),
        hasShape(hasShape) {
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