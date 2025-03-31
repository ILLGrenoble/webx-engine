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
        height(window->getRectangle().size().height()) {
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
        height(window.height) {
    }

    /**
     * @brief Destructor.
     */
    virtual ~WebXWindowProperties() {}

    /**
     * @brief Assignment operator to copy properties from another instance.
     * @param window Reference to another WebXWindowProperties instance.
     * @return Reference to the current instance.
     */
    WebXWindowProperties & operator=(const WebXWindowProperties & window) {
        if (this != &window) {
            this->id = window.id;
            this->x = window.x;
            this->y = window.y;
            this->width = window.width;
            this->height = window.height;
        }
        return *this;
    }

    /**
     * @brief X11 window ID.
     */
    unsigned long id;

    /**
     * @brief X-coordinate of the window's position.
     */
    int x;

    /**
     * @brief Y-coordinate of the window's position.
     */
    int y;

    /**
     * @brief Width of the window.
     */
    int width;

    /**
     * @brief Height of the window.
     */
    int height;
};

#endif /* WEBX_WINDOW_PROPERTIES */