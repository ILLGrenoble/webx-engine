#include "WebXManager.h"
#include "WebXDisplay.h"
#include "WebXWindow.h"
#include "WebXController.h"
#include "WebXWindowDamageProperties.h"
#include <events/WebXEventListener.h>
#include <X11/Xatom.h>
#include <stdio.h>

WebXManager * WebXManager::_instance = NULL;

int WebXManager::ERROR_HANDLER(Display *display, XErrorEvent *err) {
    fprintf(stderr, "X11 error for window with id 0x%0lx, error 0x%02x\n", err->resourceid, err->error_code);
    return 0;
}

WebXManager::WebXManager() :
    _x11Display(NULL),
    _display(NULL),
    _eventListener(NULL),
    _controller(NULL) {
}

WebXManager::~WebXManager() {
    printf("Stopping manager...\n");
    if (this->_eventListener) {
        delete this->_eventListener;
        this->_eventListener = NULL;
    }

    if (this->_controller) {
        delete this->_controller;
        this->_controller = NULL;
    }

    if (this->_display) {
        delete this->_display;
        this->_display = NULL;
    }

    if (_instance->_x11Display) {
        XCloseDisplay(_instance->_x11Display);
    }

    printf("... manager terminated\n");
}

WebXManager * WebXManager::instance() {
    if (_instance == NULL) {
        _instance = new WebXManager();
        _instance->init();
    }

    return _instance;
}

void WebXManager::init() {
    using namespace std::placeholders;

    XInitThreads();
    this->_x11Display = XOpenDisplay(NULL);
    if (this->_x11Display == NULL) {
        printf("Could not attach to the XServer. Please verify the DISPLAY environment variable.\n");
        exit(EXIT_FAILURE);
    }

    XSetErrorHandler(WebXManager::ERROR_HANDLER);
    XSynchronize(this->_x11Display, True);

    this->_display = new WebXDisplay(this->_x11Display);
    this->_display->init();

    this->_controller = new WebXController(this->_display);
    this->_controller->run();

    this->_eventListener = new WebXEventListener(this->_x11Display);
    this->_eventListener->addEventHandler(WebXEventType::Create, std::bind(&WebXManager::handleWindowCreateEvent, this, _1));
    this->_eventListener->addEventHandler(WebXEventType::Destroy, std::bind(&WebXManager::handleWindowDestroyEvent, this, _1));
    this->_eventListener->addEventHandler(WebXEventType::Map, std::bind(&WebXManager::handleWindowMapEvent, this, _1));
    this->_eventListener->addEventHandler(WebXEventType::Unmap, std::bind(&WebXManager::handleWindowUnmapEvent, this, _1));
    this->_eventListener->addEventHandler(WebXEventType::Reparent, std::bind(&WebXManager::handleWindowReparentEvent, this, _1));
    this->_eventListener->addEventHandler(WebXEventType::Configure, std::bind(&WebXManager::handleWindowConfigureEvent, this, _1));
    this->_eventListener->addEventHandler(WebXEventType::Gravity, std::bind(&WebXManager::handleWindowGravityEvent, this, _1));
    this->_eventListener->addEventHandler(WebXEventType::Circulate, std::bind(&WebXManager::handleWindowCirculateEvent, this, _1));
    this->_eventListener->addEventHandler(WebXEventType::Damaged, std::bind(&WebXManager::handleWindowDamageEvent, this, _1));
    this->_eventListener->run(this->_display->getRootWindow());
}

void WebXManager::shutdown() {
    if (_instance) {
        delete _instance;
        _instance = NULL;
    }
}

void WebXManager::pauseEventListener() {
    this->_eventListener->pause();
}

void WebXManager::resumeEventListener() {
    this->_eventListener->resume();
}

void WebXManager::handleWindowCreateEvent(const WebXEvent & event) {
    // printf("Got Create Event for window 0x%08lx\n", event.getX11Window());

}

void WebXManager::handleWindowDestroyEvent(const WebXEvent & event) {
    // printf("Got Destroy Event for window 0x%08lx\n", event.getX11Window());

}

void WebXManager::handleWindowMapEvent(const WebXEvent & event) {
    // printf("Got Map Event for window 0x%08lx\n", event.getX11Window());

    this->_display->createWindowInTree(event.getX11Window());
    this->updateDisplay();
}

void WebXManager::handleWindowUnmapEvent(const WebXEvent & event) {
    // printf("Got Unmap Event for window 0x%08lx\n", event.getX11Window());

    this->_display->removeWindowFromTree(event.getX11Window());
    this->updateDisplay();
}

void WebXManager::handleWindowReparentEvent(const WebXEvent & event) {
    // printf("Got Reparent Event for window 0x%08lx\n", event.getX11Window());

    this->_display->reparentWindow(event.getX11Window(), event.getParent());
    this->updateDisplay();
}

void WebXManager::handleWindowConfigureEvent(const WebXEvent & event) {
    this->updateDisplay();
}

void WebXManager::handleWindowGravityEvent(const WebXEvent & event) {
    printf("Got Gravity Event for window 0x%08lx\n", event.getX11Window());
}

void WebXManager::handleWindowCirculateEvent(const WebXEvent & event) {
    printf("Got Circulate Event for window 0x%08lx\n", event.getX11Window());
}

void WebXManager::handleWindowDamageEvent(const WebXEvent & event) {
    WebXWindow * window = this->_display->getWindow(event.getX11Window());

    window->setDamaged(event.getRectangle());
    this->_controller->onWindowDamaged(WebXWindowDamageProperties(window));
}

void WebXManager::updateDisplay() {
    this->_display->updateVisibleWindows();
    this->_controller->onDisplayChanged();
}
