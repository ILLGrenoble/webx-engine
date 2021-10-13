#include "WebXController.h"
#include "WebXDisplay.h"
#include "WebXManager.h"
#include "WebXConnection.h"
#include <connector/instruction/WebXMouseInstruction.h>
#include <connector/instruction/WebXKeyboardInstruction.h>
#include <image/WebXSubImage.h>
#include <input/WebXMouse.h>
#include <utils/WebXPosition.h>
#include <algorithm>
#include <thread>
#include <spdlog/spdlog.h>

unsigned int WebXController::THREAD_RATE = 60;
unsigned int WebXController::IMAGE_REFRESH_RATE = 30;
unsigned int WebXController::MOUSE_MAX_REFRESH_RATE = 60;
unsigned int WebXController::MOUSE_MIN_REFRESH_RATE = 10;

WebXController::WebXController(WebXDisplay * display) :
    _display(display),
    _displayDirty(true),
    _mouseDirty(true),
    _imageRefreshUs(1000000.0 / WebXController::IMAGE_REFRESH_RATE),
    _thread(NULL),
    _threadSleepUs(1000000.0 / WebXController::THREAD_RATE),
    _state(WebXControllerState::Stopped),
    _fpsStoreIndex(0) {

    for (int i = 0; i < WebXController::FPS_STORE_SIZE; i++) {
        this->_fpsStore.push_back(0.0);
    }
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
        spdlog::info("Stopping controller...");
        this->_thread->join();
        spdlog::info("Stopped controller");
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
    long calculateThreadSleepUs = this->_threadSleepUs;
    double mouseRefreshUs = WebXController::MOUSE_MAX_REFRESH_RATE;
    std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point lastMouseRefreshTime = lastTime;
    WebXMouse * mouse = this->_display->getMouse();

    while (this->_state != WebXControllerState::Stopped) {
        if (calculateThreadSleepUs > 0) {
            std::this_thread::sleep_for(std::chrono::microseconds(calculateThreadSleepUs));
        }

        if (this->_state != WebXControllerState::Stopped) {
            std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::micro> delayUs = start - lastTime;
            std::chrono::duration<double, std::micro> timeSinceMouseRefreshUs = start - lastMouseRefreshTime;
            lastTime = start;
            double fps = 1000000 / delayUs.count();
            this->updateFps(fps);

            // Handling of mouse movement
            WebXPosition initialMousePosition(mouse->getState()->getX(), mouse->getState()->getY());

            // Manually get mouse position after given delay: increase delay until mouse has moved again 
            if (timeSinceMouseRefreshUs.count() > mouseRefreshUs) {
                mouse->updatePosition();
                lastMouseRefreshTime = std::chrono::high_resolution_clock::now();
                mouseRefreshUs *= 0.96;
                mouseRefreshUs = mouseRefreshUs < WebXController::MOUSE_MIN_REFRESH_RATE ? WebXController::MOUSE_MIN_REFRESH_RATE : mouseRefreshUs;
            }

            // Handle all client instructions
            this->handleClientInstructions();

            // Flush all X11 events
            WebXManager::instance()->flushEventListener();

            if (this->_state != WebXControllerState::Paused) {

                if (this->_displayDirty) {
                    // Dispatch display event to connectors
                    this->notifyDisplayChanged();
                }

                // Update necessary images
                this->notifyImagesChanged();

                WebXPosition finalMousePosition(mouse->getState()->getX(), mouse->getState()->getY());
                if (this->_mouseDirty || finalMousePosition != initialMousePosition) {
                    this->notifyMouseChanged();
                    mouseRefreshUs = WebXController::MOUSE_MAX_REFRESH_RATE;
                }
            }

            std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::micro> durationUs = end - start;
            long duration = durationUs.count();
            calculateThreadSleepUs = duration > this->_threadSleepUs ? 0 : this->_threadSleepUs - duration;
        }
    }
}

void WebXController::handleClientInstructions() {
    tthread::lock_guard<tthread::mutex> lock(this->_instructionsMutex);
    for (auto it = this->_instructions.begin(); it != this->_instructions.end(); it++) {
        auto instruction = *it;
        if (instruction->type == WebXInstruction::Type::Mouse) {
            auto mouseInstruction = std::static_pointer_cast<WebXMouseInstruction>(instruction);
            WebXDisplay * display = WebXManager::instance()->getDisplay();
            display->sendClientMouseInstruction(mouseInstruction->x, mouseInstruction->y, mouseInstruction->buttonMask);
        }

        if (instruction->type == WebXInstruction::Type::Keyboard) {
           auto keyboardInstruction = std::static_pointer_cast<WebXKeyboardInstruction>(instruction);
            WebXDisplay * display = WebXManager::instance()->getDisplay();
            display->sendKeyboard(keyboardInstruction->key, keyboardInstruction->pressed);
        }
    }

    this->_instructions.clear();
}

void WebXController::notifyDisplayChanged() {
    tthread::lock_guard<tthread::mutex> windowsLock(this->_windowsMutex);
    this->_windows = this->_display->getVisibleWindowsProperties();
    this->_displayDirty = false;

    tthread::lock_guard<tthread::mutex> connectionsLock(this->_connectionsMutex);
    for (WebXConnection * connection : this->_connections) {
        connection->onDisplayChanged(this->_windows);
    }
}

void WebXController::notifyImagesChanged() {
    std::vector<WebXWindowDamageProperties> damagedWindows = this->_display->getDamagedWindows(this->_imageRefreshUs);
    if (damagedWindows.size() > 0) {
        for (auto it = damagedWindows.begin(); it != damagedWindows.end(); it++) {
            WebXWindowDamageProperties & windowDamage = *it;

            if (windowDamage.isFullWindow()) {
                // Get checksums before and after updating the window image
                uint64_t oldChecksum = this->_display->getWindowChecksum(windowDamage.windowId);
                std::shared_ptr<WebXImage> image = this->_display->getImage(windowDamage.windowId);
                if (image) {
                    uint64_t newChecksum = this->_display->getWindowChecksum(windowDamage.windowId);

                    // Send event if checksum has changed
                    if (newChecksum != oldChecksum) {
                        tthread::lock_guard<tthread::mutex> connectionsLock(this->_connectionsMutex);
                        spdlog::debug("Sending image event for window 0x{:01x}", windowDamage.windowId);

                        if (image->getRawDataSize() > 1024) {
                            spdlog::debug("Encoded image [{:d} x {:d} x {:d} @ {:d}KB ({:d}ms)]", image->getWidth(), image->getHeight(), image->getDepth(), (int)((1.0 * image->getRawDataSize()) / 1024), (int)(image->getEncodingTimeUs() / 1000));
                        } else {
                            spdlog::debug("Encoded image [{:d} x {:d} x {:d} @ {:f}KB ({:d}us)]", image->getWidth(), image->getHeight(), image->getDepth(), (1.0 * image->getRawDataSize()) / 1024, (int)(image->getEncodingTimeUs()));
                        }

                        for (WebXConnection * connection : this->_connections) {
                            connection->onImageChanged(windowDamage.windowId, image);
                        }
                    }
                }

            } else {
                // Get sub image changes

                std::vector<WebXSubImage> subImages;
                for (auto it = windowDamage.damageAreas.begin(); it != windowDamage.damageAreas.end(); it++) {
                    WebXRectangle & area = *it;
                    std::shared_ptr<WebXImage> image = this->_display->getImage(windowDamage.windowId, &area);
                    // Check image not null
                    if (image) {
                        subImages.push_back(WebXSubImage(area, image));
                    }
                }

                tthread::lock_guard<tthread::mutex> connectionsLock(this->_connectionsMutex);
                spdlog::debug("Sending subimage event for window 0x{:01x}", windowDamage.windowId);
                for (auto it = subImages.begin(); it != subImages.end(); it++) {
                    const WebXSubImage & subImage = *it;
                    if (subImage.image->getRawDataSize() > 1024) {
                        spdlog::debug("Encoded image [{:d} x {:d} x {:d} @ {:d}KB ({:d}ms)]", subImage.imageRectangle.size.width, subImage.imageRectangle.size.height, subImage.image->getDepth(), (int)((1.0 * subImage.image->getRawDataSize()) / 1024), (int)(subImage.image->getEncodingTimeUs() / 1000));
                    } else {
                        spdlog::debug("Encoded image [{:d} x {:d} x {:d} @ {:f}KB ({:d}us)]", subImage.imageRectangle.size.width, subImage.imageRectangle.size.height, subImage.image->getDepth(), (1.0 * subImage.image->getRawDataSize()) / 1024, (int)(subImage.image->getEncodingTimeUs()));
                    }
                }
                for (WebXConnection * connection : this->_connections) {
                    connection->onSubImagesChanged(windowDamage.windowId, subImages);
                }
            }
        }
    }
}

void WebXController::notifyMouseChanged() {
    this->_mouseDirty = false;

    const WebXMouseState  * mouseState = this->_display->getMouse()->getState();
    for (WebXConnection * connection : this->_connections) {
        connection->onMouseChanged(mouseState->getX(), mouseState->getY(), mouseState->getCursor()->getId());
    }
}

void WebXController::updateFps(double fps) {
    this->_fpsStore[this->_fpsStoreIndex++] = fps;
    if (this->_fpsStoreIndex == WebXController::FPS_STORE_SIZE) {
        this->_fpsStoreIndex = 0;

        double averageFps = 0;
        for (auto it = this->_fpsStore.begin(); it != this->_fpsStore.end(); it++) {
            averageFps += *it;
        }
        averageFps /= WebXController::FPS_STORE_SIZE;

        spdlog::trace("Average FPS = {:f}", averageFps);
    }
}



