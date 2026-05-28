#ifndef WEBX_RDP_POINTER_H
#define WEBX_RDP_POINTER_H

#include <image/WebXPNGImageConverter.h>
#include <memory>
#include <vector>
#include <map>
#include <mutex>
#include <freerdp/freerdp.h>

class WebXMouseCursor;

class WebXRdpPointer {
public:
    WebXRdpPointer(freerdp * instance) :
        _instance(instance) {
    }

    uint32_t x() const {
        return _x;
    }

    uint32_t y() const {
        return _y;
    }

    bool cursorDirty() const {
        return _cursorDirty;
    }

    void cursorDirty(bool dirty) {
        _cursorDirty = dirty;
    }

    const std::shared_ptr<WebXMouseCursor> cursor() {
        std::lock_guard<std::mutex> lock(this->_mutex);
        return _cursor;
    }

    const std::shared_ptr<WebXMouseCursor> cursorWithId(uint32_t id);

    void sendPosition(uint32_t x, uint32_t y);

    void sendButton(uint16_t button, bool pressed);

    void sendWheel(int delta);

    BOOL pointerNew(rdpPointer* pointer);
    void pointerFree(rdpPointer * pointer);
    BOOL pointerSet(rdpPointer* pointer);
    BOOL pointerSetNull();
    BOOL pointerSetDefault();
    BOOL pointerSetPosition(uint32_t x, uint32_t y);
 
private:
    WebXPNGImageConverter _imageConverter;
    freerdp * _instance;
    std::mutex _mutex;

    uint32_t _x = 0;
    uint32_t _y = 0;
    std::shared_ptr<WebXMouseCursor> _cursor;

    bool _cursorDirty;

    static uint32_t CURSOR_ID_COUNTER;
    std::map<rdpPointer *, std::shared_ptr<WebXMouseCursor>> _cursorCache;

};

#endif /* WEBX_RDP_POINTER_H */
