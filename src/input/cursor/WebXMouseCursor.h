#ifndef WEBX_MOUSE_CURSOR_H
#define WEBX_MOUSE_CURSOR_H

#include <memory>
#include <image/WebXImage.h>

class WebXMouseCursorImage;
class WebXMouseCursorImageConverter;

class WebXMouseCursor {
public:

    WebXMouseCursor(const std::string & name, unsigned long serial, std::shared_ptr<WebXImage> image, int xhot, int yhot);
    virtual ~WebXMouseCursor();

    /**
     * Returns a png image of the mouse cursor
     * @return  the mouse cursor image
     */
    std::shared_ptr<WebXImage> getImage() {
        return this->_image;
    }

    const std::string & getName() const {
        return this->_name;
    }

    const unsigned long getSerial() const {
        return this->_serial;
    }

    int getXhot() const {
        return this->_xhot;
    }

    int getYhot() const {
        return this->_yhot;
    }

private:
    std::string _name;
    unsigned long _serial;
    int _xhot;
    int _yhot;
    std::shared_ptr<WebXImage> _image;
};


#endif //WEBX_MOUSE_CURSOR_H
