#include "WebXManager.h"
#include "WebXDisplay.h"
#include "WebXWindow.h"
#include "WebXErrorHandler.h"
#include "WebXClipboard.h"
#include "events/WebXEventListener.h"
#include <X11/extensions/Xfixes.h>

#include <stdio.h>
#include <spdlog/spdlog.h>

int WebXManager::IO_ERROR_HANDLER(Display *display) {
    spdlog::error("X11 quit unexpectedly");
    return 0;
}

WebXManager::WebXManager(const WebXSettings & settings, const std::string & keyboardLayout) :
    _settings(settings),
    _x11Display(NULL),
    _display(NULL),
    _eventListener(NULL),
    _clipboard(NULL),
    _displayRequiresUpdate(false) {

    this->init(keyboardLayout);
}

WebXManager::~WebXManager() {
    spdlog::info("Stopping manager...");
    if (this->_clipboard) {
        delete this->_clipboard;
        this->_clipboard = NULL;
    }

    if (this->_eventListener) {
        delete this->_eventListener;
        this->_eventListener = NULL;
    }

    if (this->_display) {
        delete this->_display;
        this->_display = NULL;
    }

    if (this->_x11Display) {
        XCloseDisplay(this->_x11Display);
    }

    spdlog::info("Manager terminated");
}


void WebXManager::init(const std::string & keyboardLayout) {
    using namespace std::placeholders;

    XInitThreads();
    this->_x11Display = XOpenDisplay(NULL);
    if (this->_x11Display == NULL) {
        spdlog::error("Could not attach to the XServer. Please verify the DISPLAY environment variable.");
        exit(EXIT_FAILURE);
    }

    XSetErrorHandler(WebXErrorHandler::setLastError);
    XSetIOErrorHandler(WebXManager::IO_ERROR_HANDLER);
    XSynchronize(this->_x11Display, True);

    this->_display = new WebXDisplay(this->_x11Display);
    this->_display->init();

    this->_clipboard = new WebXClipboard(this->_x11Display, this->_display->getRootWindow()->getX11Window(), [this](const std::string & content) {
        this->sendClipboardEvent(content);
    });

    this->_eventListener = new WebXEventListener(this->_settings, this->_x11Display, this->_display->getRootWindow()->getX11Window());
    
    this->_eventListener->setMapEventHandler([this](const WebXMapEvent & event) {
        spdlog::trace("Got Map Event for window 0x{:x} {:d}", event.getWindow(), event.getSerial());
        this->_display->createWindowInTree(event.getWindow());
        this->_displayRequiresUpdate = true;
    });
    
    this->_eventListener->setUnmapEventHandler([this](const WebXUnmapEvent & event) {
        spdlog::trace("Got Unmap Event for window 0x{:x}", event.getWindow());
        this->_display->removeWindowFromTree(event.getWindow());
        this->_displayRequiresUpdate = true;
    });
    
    this->_eventListener->setReparentEventHandler([this](const WebXReparentEvent & event) {
        spdlog::trace("Got Reparent Event for window 0x{:x}", event.getWindow());
        this->_display->reparentWindow(event.getWindow(), event.getParentWindow());
        this->_displayRequiresUpdate = true;
    });
    
    this->_eventListener->setConfigureEventHandler([this](const WebXConfigureEvent & event) {
        spdlog::trace("Got Configure Event for window 0x{:x}", event.getWindow());
        this->handleWindowConfigureEvent(event);
    });
    
    this->_eventListener->setDamageEventHandler([this](const WebXDamageEvent & event) {
        spdlog::trace("Got damage Event for window 0x{:x} {:d}", event.getWindow(), event.getSerial());
        this->sendDamageEvent(WebXWindowDamage(event.getWindow(), event.getRectangle()));
    });

    this->_eventListener->setCursorEventHandler([this](const WebXCursorEvent & event) {
        spdlog::trace("Got new mouse cursor event");
        this->_display->updateMouseCursor();
        this->sendDisplayEvent(CursorEvent);
    });
    
    this->_eventListener->setSelectionEventHandler([this](const WebXSelectionEvent & event) {
        this->_clipboard->onClipboardDataNotify(event.getXSelectionEvent());
    });

    this->_eventListener->setSelectionRequestEventHandler([this](const WebXSelectionRequestEvent & event) {
        this->_clipboard->onClipboardContentRequest(event.getXSelectionRequestEvent());
    });

    this->_display->loadKeyboardLayout(keyboardLayout);
}

void WebXManager::handlePendingEvents() {
    this->_clipboard->updateClipboard();
    this->_eventListener->flushQueuedEvents();
    this->updateDisplay();
}

void WebXManager::setClipboardContent(const std::string & clipboardContent) {
    this->_clipboard->setClipboardContent(clipboardContent);
}

void WebXManager::handleWindowConfigureEvent(const WebXConfigureEvent & event) {
    WebXWindow * window = this->_display->getWindow(event.getWindow());
    if (window != NULL) {
        const WebXSize & windowSize = window->getRectangle().size();
        bool sizeHasChanged = windowSize.width() != event.getWidth() || windowSize.height() != event.getHeight();

        if (sizeHasChanged) {
            // Send this as an event to indicate that the full window is damaged
            this->sendDamageEvent(WebXWindowDamage(event.getWindow(), window->getRectangle(), true));
        }
    }

    this->_displayRequiresUpdate = true;
}

void WebXManager::updateDisplay() {
    if (this->_displayRequiresUpdate) {
        this->_display->updateVisibleWindows();
        this->sendDisplayEvent(WindowLayoutEvent);
        this->_displayRequiresUpdate = false;
    }
}

