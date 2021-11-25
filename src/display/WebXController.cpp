#include "WebXController.h"
#include "WebXDisplay.h"
#include "WebXManager.h"
#include "WebXConnection.h"
#include <connector/instruction/WebXMouseInstruction.h>
#include <connector/instruction/WebXKeyboardInstruction.h>
#include <connector/instruction/WebXImageInstruction.h>
#include <connector/instruction/WebXCursorImageInstruction.h>
#include <connector/message/WebXScreenMessage.h>
#include <connector/message/WebXWindowsMessage.h>
#include <connector/message/WebXImageMessage.h>
#include <connector/message/WebXCursorImageMessage.h>
#include <connector/message/WebXSubImagesMessage.h>
#include <connector/message/WebXMouseMessage.h>
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
    _connection(NULL),
    _fpsStoreIndex(0) {

    for (int i = 0; i < WebXController::FPS_STORE_SIZE; i++) {
        this->_fpsStore.push_back(0.0);
    }
}

WebXController::~WebXController() {
    this->stop();
}

void WebXController::run() {
    std::lock_guard<std::mutex> lock(this->_stateMutex);
    this->_state = WebXControllerState::Running;
    if (this->_thread == NULL) {
        this->_thread = new std::thread(&WebXController::mainLoop, this);
    }
}

void WebXController::stop() {
    std::lock_guard<std::mutex> lock(this->_stateMutex);
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
    std::lock_guard<std::mutex> lock(this->_stateMutex);
    if (this->_state == WebXControllerState::Running) {
        this->_state = WebXControllerState::Paused;
    }
}

void WebXController::resume() {
    std::lock_guard<std::mutex> lock(this->_stateMutex);
    if (this->_state == WebXControllerState::Paused) {
        this->_state = WebXControllerState::Running;
    }
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
    std::lock_guard<std::mutex> lock(this->_instructionsMutex);
    for (auto it = this->_instructions.begin(); it != this->_instructions.end(); it++) {
        auto instruction = *it;
        if (instruction->type == WebXInstruction::Type::Mouse) {
            auto mouseInstruction = std::static_pointer_cast<WebXMouseInstruction>(instruction);
            WebXDisplay * display = WebXManager::instance()->getDisplay();
            display->sendClientMouseInstruction(mouseInstruction->x, mouseInstruction->y, mouseInstruction->buttonMask);
        
        } else if (instruction->type == WebXInstruction::Type::Keyboard) {
           auto keyboardInstruction = std::static_pointer_cast<WebXKeyboardInstruction>(instruction);
            WebXDisplay * display = WebXManager::instance()->getDisplay();
            display->sendKeyboard(keyboardInstruction->key, keyboardInstruction->pressed);
    
        } else if (instruction->type == WebXInstruction::Type::Screen) {
            auto message = std::make_shared<WebXScreenMessage>(WebXManager::instance()->getDisplay()->getScreenSize());
            this->sendMessage(message, instruction->id);

        } else if (instruction->type == WebXInstruction::Type::Windows) {
            auto message = std::make_shared<WebXWindowsMessage>(WebXManager::instance()->getController()->getWindows());
            this->sendMessage(message, instruction->id);
        
        } else if (instruction->type == WebXInstruction::Type::Image) {
            auto imageInstruction = std::static_pointer_cast<WebXImageInstruction>(instruction);
            std::shared_ptr<WebXImage> image = WebXManager::instance()->getDisplay()->getImage(imageInstruction->windowId);

            auto message = std::make_shared<WebXImageMessage>(imageInstruction->windowId, image);
            this->sendMessage(message, instruction->id);

        } else if (instruction->type == WebXInstruction::Type::Cursor) {
            auto cursorImageInstruction = std::static_pointer_cast<WebXCursorImageInstruction>(instruction);

            WebXMouse * mouse = WebXManager::instance()->getDisplay()->getMouse();
            WebXMouseState * mouseState = mouse->getState();
            std::shared_ptr<WebXMouseCursor> mouseCursor = mouse->getCursor(cursorImageInstruction->cursorId);
            
            auto message = std::make_shared<WebXCursorImageMessage>(mouseState->getX(), mouseState->getY(), mouseCursor->getXhot(), mouseCursor->getYhot(), mouseCursor->getId(), mouseCursor->getImage());
            this->sendMessage(message, instruction->id);
        }
    }

    this->_instructions.clear();
}

void WebXController::sendMessage(std::shared_ptr<WebXMessage> message, uint32_t commandId) {
    message->commandId = commandId;
    std::lock_guard<std::mutex> connectionsLock(this->_connectionsMutex);

    this->_connection->onMessage(message);
}

void WebXController::notifyDisplayChanged() {
    std::lock_guard<std::mutex> windowsLock(this->_windowsMutex);
    this->_windows = this->_display->getVisibleWindowsProperties();
    this->_displayDirty = false;

    auto message = std::make_shared<WebXWindowsMessage>(this->_windows);

    this->sendMessage(message);
}

void WebXController::notifyImagesChanged() {
    std::vector<WebXWindowDamageProperties> damagedWindows = this->_display->getDamagedWindows(this->_imageRefreshUs);
    if (damagedWindows.size() > 0) {
        for (auto it = damagedWindows.begin(); it != damagedWindows.end(); it++) {
            WebXWindowDamageProperties & windowDamage = *it;

            if (windowDamage.isFullWindow() || windowDamage.getDamageAreaRatio() > 0.9) {
                WebXWindow * window = this->_display->getVisibleWindow(windowDamage.windowId);
                if (window) {
                    // Get checksums before and after updating the window image
                    uint64_t oldChecksum = window->getWindowChecksum();
                    uint64_t oldAlphaChecksum = window->getWindowAlphaChecksum();
                    std::shared_ptr<WebXImage> image = this->_display->getImage(windowDamage.windowId);
                    if (image) {
                        uint64_t newChecksum = window->getWindowChecksum();
                        uint64_t newAlphaChecksum = window->getWindowAlphaChecksum();

                        // Send event if checksum has changed
                        if (newChecksum != oldChecksum) {

                            // Compare alpha checksums
                            if (newAlphaChecksum == oldAlphaChecksum) {
                                bool removed = image->removeAlpha();
                                if (removed) {
                                    spdlog::debug("Removing alpha from image for window 0x{:01x}", windowDamage.windowId);
                                }
                            }

                            spdlog::debug("Sending encoded image {:d} x {:d} x {:d} @ {:d}KB ({:d}ms)", image->getWidth(), image->getHeight(), image->getDepth(), (int)((1.0 * image->getFullDataSize()) / 1024), (int)(image->getEncodingTimeUs() / 1000));

                            auto message = std::make_shared<WebXImageMessage>(windowDamage.windowId, image);
                            this->sendMessage(message);
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

                if (subImages.size() > 0) {
                    for (auto it = subImages.begin(); it != subImages.end(); it++) {
                        const WebXSubImage & subImage = *it;
                        spdlog::debug("Sending encoded subimage {:d} x {:d} x {:d} @ {:d}KB ({:d}ms)", subImage.imageRectangle.size.width, subImage.imageRectangle.size.height, subImage.image->getDepth(), (int)((1.0 * subImage.image->getFullDataSize()) / 1024), (int)(subImage.image->getEncodingTimeUs() / 1000));
                    }
    
                    auto message = std::make_shared<WebXSubImagesMessage>(windowDamage.windowId, subImages);
                    this->sendMessage(message);
                }
            }
        }
    }
}

void WebXController::notifyMouseChanged() {
    this->_mouseDirty = false;

    const WebXMouseState  * mouseState = this->_display->getMouse()->getState();

    auto message = std::make_shared<WebXMouseMessage>( mouseState->getX(), mouseState->getY(), mouseState->getCursor()->getId());
    this->sendMessage(message);
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



