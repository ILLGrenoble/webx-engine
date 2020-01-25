#include "WebXMouse.h"
#include "WebXMouseState.h"
#include <X11/X.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/XTest.h>
#include <image/WebXPNGImageConverter.h>

WebXMouse::WebXMouse(Display * x11Display, Window rootWindow) :
    _x11Display(x11Display),
    _rootWindow(rootWindow),
    _imageConverter(new WebXPNGImageConverter()),
    _currentMouseState(createDefaultMouseState()) {
}

WebXMouse::~WebXMouse() {
    delete _currentMouseState;
    delete _imageConverter;
}

void WebXMouse::sendClientInstruction(int x, int y, unsigned int buttonMask) {
    sendMouseMovement(x, y);
    sendMouseButtons(buttonMask);
    _currentMouseState->setState(x, y, buttonMask);
}

void WebXMouse::sendMouseButtons(unsigned int newButtonMask) {
    int currentButtonMask = _currentMouseState->getButtonMask();
    int buttonMaskDelta = currentButtonMask ^newButtonMask;
    unsigned int buttonMasks[5] = {LeftButtonMask, MiddleButtonMask, RightButtonMask, ScrollUpButtonMask,
                                   ScrollDownButtonMask};
    unsigned int buttons[5] = {LeftButton, MiddleButton, RightButton, ScrollUpButton, ScrollDownButton};
    for (int i = 0; i <= 4; i++) {
        // Check if the state has changed for a given button
        if (buttonMaskDelta & buttonMasks[i]) {
            // update the button if changed
            sendMouseButton(buttons[i], newButtonMask & buttonMasks[i]);
        }
    }
    // reset the mask
    _currentMouseState->setButtonMask(0);
}

void WebXMouse::sendMouseButton(unsigned int button, Bool isPressed) {
    XTestFakeButtonEvent(_x11Display, button, isPressed, 0);
}

void WebXMouse::sendMouseMovement(int newX, int newY) {
    int currentX = _currentMouseState->getX();
    int currentY = _currentMouseState->getY();
    if (newX != currentX || newY != currentY) {
        XWarpPointer(_x11Display, 0L, _rootWindow, 0, 0, 0, 0, newX, newY);
    }
}

void WebXMouse::updateCursor() {
    WebXMouseCursor * newCursor = createCursor();
    if (newCursor) {
        _currentMouseState->setCursor(newCursor);
    }
}

WebXMouseCursor * WebXMouse::createCursor() {
    XFixesCursorImage * cursorImage = XFixesGetCursorImage(_x11Display);
    if (cursorImage) {
        WebXImage * image = convertCursorImage(cursorImage);
        if (image != NULL) {
            return new WebXMouseCursor(cursorImage->name, cursorImage->cursor_serial, std::shared_ptr<WebXImage>(image), (int)cursorImage->xhot, (int)cursorImage->yhot);
        }
    }
    return NULL;
}

WebXMouseState * WebXMouse::createDefaultMouseState() {
    WebXMouseCursor * cursor = createCursor();
    return new WebXMouseState(cursor);
}

WebXImage * WebXMouse::convertCursorImage(XFixesCursorImage * cursorImage) {
    // Convert raw image data to WebXImage
    unsigned int imageByteLength = cursorImage->width * cursorImage->height * 4;
    unsigned char * imageData = (unsigned char *)malloc(imageByteLength);
    unsigned long * src = cursorImage->pixels;
    unsigned int offset = 0;
    while (offset < imageByteLength) {
        uint32_t p = *src++;
        uint8_t r = p >> 0;
        uint8_t g = p >> 8;
        uint8_t b = p >> 16;
        uint8_t a = p >> 24;

        if (a > 0x00 && a < 0xff) {
            r = (r * 0xff + a / 2) / a;
            g = (g * 0xff + a / 2) / a;
            b = (b * 0xff + a / 2) / a;
        }

        imageData[offset + 0] = b;
        imageData[offset + 1] = g;
        imageData[offset + 2] = r;
        imageData[offset + 3] = a;

        offset += 4;
    }

    WebXImage * image = this->_imageConverter->convert(imageData, (int)cursorImage->width, (int)cursorImage->height, (int)cursorImage->width * 4, 32);
    free(imageData);

    return image;
}





