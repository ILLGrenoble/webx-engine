#include "WebXRdpPointer.h"
#include <display/input/cursor/WebXMouseCursor.h>
#include <spdlog/spdlog.h>

uint32_t WebXRdpPointer::CURSOR_ID_COUNTER = 1;

const std::shared_ptr<WebXMouseCursor> WebXRdpPointer::cursorWithId(uint32_t id) {
    std::lock_guard<std::mutex> lock(this->_mutex);

    for (auto it = this->_cursorCache.begin(); it != this->_cursorCache.end(); it++) {
        auto cursor = it->second;
        if (cursor->getId() == id) {
            return cursor;
        }
    }
    return nullptr;
}

void WebXRdpPointer::sendPosition(uint32_t x, uint32_t y) {
    freerdp_input_send_mouse_event(this->_instance->context->input, PTR_FLAGS_MOVE, x, y);
    this->_x = x;
    this->_y = y;
}

void WebXRdpPointer::sendButton(uint16_t button, bool pressed) {
    uint16_t flags = button;
    if (pressed) {
        flags |= PTR_FLAGS_DOWN;
    }
    freerdp_input_send_mouse_event(this->_instance->context->input, flags, this->_x, this->_y);
}

void WebXRdpPointer::sendWheel(int delta) {
    uint16_t flags = PTR_FLAGS_WHEEL;
    if (delta < 0) {
        flags |= PTR_FLAGS_WHEEL_NEGATIVE;
        delta = -delta;
    }
    flags |= (delta & WheelRotationMask);
    freerdp_input_send_mouse_event(this->_instance->context->input, flags, 0, 0);
}

BOOL WebXRdpPointer::pointerNew(rdpPointer * pointer) {
    std::lock_guard<std::mutex> lock(this->_mutex);

    uint8_t * imageData = (uint8_t *)malloc(pointer->width * pointer->height * 4);

    // freerdp_image_copy_from_pointer_data converts XOR+AND masks
    // into a straight BGRA bitmap
    BOOL copyOk = freerdp_image_copy_from_pointer_data(
            imageData,
            PIXEL_FORMAT_BGRA32,
            pointer->width * 4,
            0, 0,
            pointer->width, pointer->height,
            pointer->xorMaskData, pointer->lengthXorMask,
            pointer->andMaskData, pointer->lengthAndMask,
            pointer->xorBpp, nullptr);

    if (copyOk) {
        std::shared_ptr<WebXImage> image = std::shared_ptr<WebXImage>(this->_imageConverter.convert(imageData, pointer->width, pointer->height, pointer->width * 4, 32, WebXQuality::MaxQuality()));
        std::shared_ptr<WebXMouseCursor> cursor = std::shared_ptr<WebXMouseCursor>(new WebXMouseCursor(CURSOR_ID_COUNTER++, image, pointer->xPos, pointer->yPos));
        this->_cursorCache[pointer] = cursor;

    } else {
        spdlog::error("Failed to create rdp cursor");
    }
    free(imageData);

    spdlog::info("Got new pointer {}", copyOk);

    return copyOk;
}

void WebXRdpPointer::pointerFree(rdpPointer * pointer) {
}

BOOL WebXRdpPointer::pointerSet(rdpPointer* pointer) {
    std::lock_guard<std::mutex> lock(this->_mutex);

    auto it = this->_cursorCache.find(pointer);
    if (it == _cursorCache.end()) {
        spdlog::error("Failed to set pointer from rdp");

        return FALSE;
    }

    this->_cursor = it->second;
    this->_cursorDirty = true;

    spdlog::info("Set pointer cursor");

    return TRUE;
}

BOOL WebXRdpPointer::pointerSetNull() {
    // Hide cursor  

    this->_cursorDirty = true;
    return TRUE;
}

BOOL WebXRdpPointer::pointerSetDefault() {
    // Define a default cursor

    this->_cursorDirty = true;
    return TRUE;
}

BOOL WebXRdpPointer::pointerSetPosition(uint32_t x, uint32_t y) {
    this->_x = x;
    this->_y = y;

    spdlog::info("Set pointer position to {}x{}", x, y);

    return TRUE;
}
