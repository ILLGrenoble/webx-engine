#ifndef WEBX_MOUSE_CURSOR_H
#define WEBX_MOUSE_CURSOR_H

#include <memory>
#include <image/WebXImage.h>

class WebXMouseCursorImage;
class WebXMouseCursorImageConverter;

class WebXMouseCursor {
public:

    WebXMouseCursor(std::shared_ptr<WebXImage> image, int xhot, int yhot);
    virtual ~WebXMouseCursor();

    /**
     * Returns a png image of the mouse cursor
     * @return  the mouse cursor image
     */
    std::shared_ptr<WebXImage> getImage() {
        return this->_image;
    }

    int getXhot() const {
        return this->_xhot;
    }

    int getYhot() const {
        return this->_yhot;
    }

    uint32_t getChecksum() {
        return this->_image->getChecksum();
    }

private:
    int _xhot;
    int _yhot;
    std::shared_ptr<WebXImage> _image;
};


#endif //WEBX_MOUSE_CURSOR_H
