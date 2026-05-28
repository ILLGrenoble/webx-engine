#include "WebXRdpManager.h"
#include "WebXRdpDisplay.h"
#include <image/WebXPixelBuffer.h>
#include <spdlog/spdlog.h>

WebXRdpManager::WebXRdpManager(const WebXSettings & settings) :
    _rdpClient(nullptr),
    _display(nullptr),
    _onDisplayEvent([](WebXDisplayEventType eventType) {}),
    _onDamageEvent([](const WebXWindowDamage & damage) {}),
    _onClipboardEvent([](const std::string & clipboardContent) {}),
    _onScreenResizeEvent([](int width, int height) {}) {
    this->init();
}

WebXRdpManager::~WebXRdpManager() {

    if (this->_display) {
        delete this->_display;
        this->_display = nullptr;
    }

    if (this->_rdpClient) {
        delete this->_rdpClient;
        this->_rdpClient = nullptr;
    }
}

void WebXRdpManager::init() {
    WebXRdpClientSettings rdpClientSettings("localhost", 3389, "mario", "mario", 1920, 1080, "fr");

    this->_rdpClient = new WebXRdpClient();
    if (!this->_rdpClient->connect(rdpClientSettings)) {
        exit(EXIT_FAILURE);
    }

    this->_rdpClient->start();

    this->_display = new WebXRdpDisplay(this->_rdpClient);
    this->_display->init();

    this->_rdpClient->setCursorEventHandler([this]() {
        spdlog::trace("Got new mouse cursor event");
        this->_display->updateMouseCursor();
        this->sendDisplayEvent(CursorEvent);
    });
    
    this->_rdpClient->setFramebufferEventHandler([this](const std::vector<WebXRectangle> & invalidRectangles, const WebXPixelBuffer & framebuffer) {
        this->_display->updateFramebuffer(invalidRectangles, framebuffer);
        this->handleInvalidRectangles(invalidRectangles);
    });

    this->_rdpClient->setResizeEventHandler([this](uint32_t desktopWidth, uint32_t desktopHeight) {
        spdlog::trace("Got screen resize event of {}x{}", desktopWidth, desktopHeight);
        this->sendScreenResizeEvent(desktopWidth, desktopHeight);
    });

}

void WebXRdpManager::handleInvalidRectangles(const std::vector<WebXRectangle> & invalidRectangles) {
    for (auto it = invalidRectangles.begin(); it != invalidRectangles.end(); it++) {
        this->sendDamageEvent(WebXWindowDamage(0, *it));
    }
}


WebXDisplay * WebXRdpManager::getDisplay() const {
    return this->_display;
}

void WebXRdpManager::handlePendingEvents() {
    this->_rdpClient->flushEvents();
}

void WebXRdpManager::setClipboardContent(const std::string & clipboardContent) {

}

