#include "WebXController.h"
#include "WebXDisplay.h"
#include "WebXConnection.h"
#include <algorithm>
#include <thread>

unsigned int WebXController::THREAD_RATE = 120;
unsigned int WebXController::DISPLAY_REFRESH_RATE = 60;

WebXController::WebXController(WebXDisplay * display) :
    _display(display),
    _displayDirty(true),
    _displayRefreshUs(1000000.0 / WebXController::DISPLAY_REFRESH_RATE),
    _lastDisplayRefreshTime(std::chrono::high_resolution_clock::now()),
    _thread(NULL),
    _threadSleepUs(1000000.0 / WebXController::DISPLAY_REFRESH_RATE),
    _state(WebXControllerState::Stopped) {
}

WebXController::~WebXController() {
    this->stop();
}

void WebXController::run() {
    tthread::lock_guard<tthread::mutex> lock(this->_stateMutex);
    this->_state = WebXControllerState::Running;
    if (this->_thread == NULL) {
        this->_thread = new tthread::thread(WebXController::threadMain, (void *)this);
    }
}

void WebXController::stop() {
    tthread::lock_guard<tthread::mutex> lock(this->_stateMutex);
    this->_state = WebXControllerState::Stopped;
    if (this->_thread != NULL) {
        // Join thread and cleanup
        printf("Stopping controller...\n");
        this->_thread->join();
        printf("... stopped controller\n");
        delete this->_thread;
        this->_thread = NULL;
    }
}

void WebXController::pause() {
    tthread::lock_guard<tthread::mutex> lock(this->_stateMutex);
    if (this->_state == WebXControllerState::Running) {
        this->_state = WebXControllerState::Paused;
    }
}

void WebXController::resume() {
    tthread::lock_guard<tthread::mutex> lock(this->_stateMutex);
    if (this->_state == WebXControllerState::Paused) {
        this->_state = WebXControllerState::Running;
    }
}

void WebXController::threadMain(void * arg) {
    WebXController * self  = (WebXController *)arg;
    self->mainLoop();
}

void WebXController::mainLoop() {
    while (this->_state != WebXControllerState::Stopped) {
        std::this_thread::sleep_for(std::chrono::microseconds(this->_threadSleepUs));

        if (this->_state != WebXControllerState::Paused) {
            
            std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::micro> timeSinceDisplayUpdateUs = now - this->_lastDisplayRefreshTime;
            if (timeSinceDisplayUpdateUs.count() >= this->_displayRefreshUs && this->_displayDirty) {
                // Dispatch display event to connectors
                this->updateDisplay();
                this->_lastDisplayRefreshTime = now;
            }
        }
    }
}

void WebXController::updateDisplay() {
    tthread::lock_guard<tthread::mutex> windowsLock(this->_windowsMutex);
    this->_windows = this->_display->getVisibleWindowsProperties();
    this->_displayDirty = false;

    tthread::lock_guard<tthread::mutex> connectionsLock(this->_connectionsMutex);
    for (WebXConnection * connection : this->_connections) {
        connection->onDisplayChanged(this->_windows);
    }
}

