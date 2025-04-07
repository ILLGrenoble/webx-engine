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

    this->_eventListener = new WebXEventListener(this->_x11Display, this->_display->getRootWindow()->getX11Window());
    this->_eventListener->addEventHandler(WebXEventType::Map, std::bind(&WebXManager::handleWindowMapEvent, this, _1));
    this->_eventListener->addEventHandler(WebXEventType::Unmap, std::bind(&WebXManager::handleWindowUnmapEvent, this, _1));
    this->_eventListener->addEventHandler(WebXEventType::Reparent, std::bind(&WebXManager::handleWindowReparentEvent, this, _1));
    this->_eventListener->addEventHandler(WebXEventType::Configure, std::bind(&WebXManager::handleWindowConfigureEvent, this, _1));
    this->_eventListener->addEventHandler(WebXEventType::Damaged, std::bind(&WebXManager::handleWindowDamageEvent, this, _1));
    this->_eventListener->addEventHandler(WebXEventType::MouseCursor, std::bind(&WebXManager::handleMouseCursorEvent, this, _1));
    this->_eventListener->addEventHandler(WebXEventType::ClipboardNotify, [this](const WebXEvent & event) {
        XSelectionEvent * selectionEvent = (XSelectionEvent *)&(event.getX11Event().xselection);
        this->_clipboard->onClipboardDataNotify(selectionEvent);
    });
    this->_eventListener->addEventHandler(WebXEventType::ClipboardContentRequest, [this](const WebXEvent & event) {
        XSelectionRequestEvent * selectionRequestEvent = (XSelectionRequestEvent *)&(event.getX11Event().xselectionrequest);
        this->_clipboard->onClipboardContentRequest(selectionRequestEvent);
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

void WebXManager::handleWindowMapEvent(const WebXEvent & event) {
    spdlog::trace("Got Map Event for window 0x{:x}", event.getX11Window());

    this->_display->createWindowInTree(event.getX11Window());
    this->_displayRequiresUpdate = true;
}

void WebXManager::handleWindowUnmapEvent(const WebXEvent & event) {
    spdlog::trace("Got Unmap Event for window 0x{:x}", event.getX11Window());

    this->_display->removeWindowFromTree(event.getX11Window());
    this->_displayRequiresUpdate = true;
}

void WebXManager::handleWindowReparentEvent(const WebXEvent & event) {
    spdlog::trace("Got Reparent Event for window 0x{:x}", event.getX11Window());

    this->_display->reparentWindow(event.getX11Window(), event.getParent());
    this->_displayRequiresUpdate = true;
}

void WebXManager::handleWindowConfigureEvent(const WebXEvent & event) {
    spdlog::trace("Got Configure Event for window 0x{:x}", event.getX11Window());

    WebXWindow * window = this->_display->getWindow(event.getX11Window());
    if (window != NULL) {
        const WebXSize & windowSize = window->getRectangle().size();
        bool sizeHasChanged = windowSize.width() != event.getWidth() || windowSize.height() != event.getHeight();

        if (sizeHasChanged) {
            // Send this as an event to indicate that the full window is damaged
            this->sendDamageEvent(WebXWindowDamage(event.getX11Window(), window->getRectangle(), true));
        }
    }

    this->_displayRequiresUpdate = true;
}

void WebXManager::handleWindowDamageEvent(const WebXEvent & event) {
    spdlog::trace("Got damage Event for window 0x{:x}: ({:d}, {:d}) {:d} x {:d}", event.getX11Window(), event.getRectangle().x(), event.getRectangle().y(), event.getRectangle().size().width(), event.getRectangle().size().height());

    // Send this as an event to indicate that a window rectangle is damaged
    this->sendDamageEvent(WebXWindowDamage(event.getX11Window(), event.getRectangle()));
}

void WebXManager::handleMouseCursorEvent(const WebXEvent & event) {
    spdlog::trace("Got new mouse cursor event");
    this->_display->updateMouseCursor();
    this->sendDisplayEvent(CursorEvent);
}

void WebXManager::updateDisplay() {
    if (this->_displayRequiresUpdate) {
        this->_display->updateVisibleWindows();
        this->sendDisplayEvent(WindowLayoutEvent);
        this->_displayRequiresUpdate = false;
    }
}

