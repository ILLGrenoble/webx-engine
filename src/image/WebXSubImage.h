#ifndef WEBX_SUB_IMAGE_H
#define WEBX_SUB_IMAGE_H

#include <memory>
#include <models/WebXRectangle.h>

class WebXImage;

/**
 * @class WebXSubImage
 * @brief Represents a sub-image of a window, including its rectangle and associated image data.
 * 
 * This class is used to encapsulate information about a sub-region of an image,
 * including its position and size (rectangle) and the corresponding image data.
 */
class WebXSubImage {
public:
    /**
     * @brief Constructs a WebXSubImage object.
     * 
     * @param imageRectangle The rectangle representing the position and size of the sub-image.
     * @param image A shared pointer to the image data associated with the sub-image.
     */
    WebXSubImage(const WebXRectangle & imageRectangle, std::shared_ptr<WebXImage> image) :
        imageRectangle(imageRectangle),
        image(image) {
    }

    /**
     * @brief Destructor for WebXSubImage.
     */
    virtual ~WebXSubImage() {}

    /**
     * @brief The rectangle representing the position and size of the sub-image.
     */
    WebXRectangle imageRectangle;

    /**
     * @brief A shared pointer to the image data associated with the sub-image.
     */
    std::shared_ptr<WebXImage> image;
};

#endif /* WEBX_SUB_IMAGE_H */