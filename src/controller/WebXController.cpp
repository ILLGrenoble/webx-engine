#include "WebXController.h"
#include <display/WebXDisplay.h>
#include <instruction/WebXMouseInstruction.h>
#include <instruction/WebXKeyboardInstruction.h>
#include <instruction/WebXImageInstruction.h>
#include <instruction/WebXCursorImageInstruction.h>
#include <instruction/WebXQualityInstruction.h>
#include <message/WebXScreenMessage.h>
#include <message/WebXWindowsMessage.h>
#include <message/WebXImageMessage.h>
#include <message/WebXCursorImageMessage.h>
#include <message/WebXSubImagesMessage.h>
#include <message/WebXMouseMessage.h>
#include <image/WebXSubImage.h>
#include <input/WebXMouse.h>
#include <utils/WebXPosition.h>
#include <utils/WebXSettings.h>
#include <algorithm>
#include <thread>
#include <spdlog/spdlog.h>

WebXController::WebXController(WebXGateway & gateway, const WebXSettings & settings, const std::string & keyboardLayout) :
    _gateway(gateway),
    _settings(settings),
    _manager(settings, keyboardLayout),
    _displayDirty(true),
    _cursorDirty(true),
    _requestedQuality(WebXQuality::MaxQuality()),
    _threadSleepUs(1000000.0 / WebXController::THREAD_RATE),
    _state(WebXControllerState::Stopped) {

    // Set the instruction handler function in the gateway
    this->_gateway.setInstructionHandlerFunc([this](std::shared_ptr<WebXInstruction> instruction) {
        const std::lock_guard<std::mutex> lock(this->_instructionsMutex);
        this->_instructions.push_back(instruction);
    });

    // Listen to events from the display
    this->_manager.setDisplayEventHandler([this](WebXDisplayEventType eventType) { this->onDisplayEvent(eventType); });
}

WebXController::~WebXController() {
}

void WebXController::stop() {
    spdlog::info("Shutdown");
    this->_state = WebXControllerState::Stopped;

    // Remove the instruction handler
    this->_gateway.setInstructionHandlerFunc(nullptr);

    // Remove the display events listener
    this->_manager.setDisplayEventHandler(nullptr);
}

void WebXController::run() {
    long calculateThreadSleepUs = this->_threadSleepUs;
    double mouseRefreshUs = WebXController::MOUSE_MIN_REFRESH_DELAY_US;
    std::chrono::high_resolution_clock::time_point lastTime = std::chrono::high_resolution_clock::now();
    std::chrono::high_resolution_clock::time_point lastMouseRefreshTime = lastTime;

    WebXDisplay * display = this->_manager.getDisplay();
    WebXMouse * mouse = display->getMouse();

    this->_state = WebXControllerState::Running;
    while (this->_state != WebXControllerState::Stopped) {
        if (calculateThreadSleepUs > 0) {
            std::this_thread::sleep_for(std::chrono::microseconds(calculateThreadSleepUs));
        }

        if (this->_state != WebXControllerState::Stopped) {
            std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::micro> delayUs = start - lastTime;
            std::chrono::duration<double, std::micro> timeSinceMouseRefreshUs = start - lastMouseRefreshTime;
            lastTime = start;

            // Handling of mouse movement
            WebXPosition initialMousePosition(mouse->getState()->getX(), mouse->getState()->getY());

            // Manually get mouse position after given delay (this is only useful if mouse is moved by something other than this webx-engine instance)
            // Increase delay until mouse has moved again to avoid updating every frame: update quickyl only if movement has just occurred.
            if (timeSinceMouseRefreshUs.count() > mouseRefreshUs) {
                mouse->updatePosition();
                lastMouseRefreshTime = std::chrono::high_resolution_clock::now();
                mouseRefreshUs *= 1.2;
                mouseRefreshUs = mouseRefreshUs > WebXController::MOUSE_MAX_REFRESH_DELAY_US ? WebXController::MOUSE_MAX_REFRESH_DELAY_US : mouseRefreshUs;
            }

            // Handle all client instructions
            this->handleClientInstructions(display);

            // Handle all pending X11 events
            this->_manager.handlePendingEvents();

            if (this->_displayDirty) {
                // Dispatch display event to connectors
                this->notifyDisplayChanged(display);
            }

            // Update necessary images
            float imageSizeKB = this->notifyImagesChanged(display);

            WebXPosition finalMousePosition(mouse->getState()->getX(), mouse->getState()->getY());
            if (this->_cursorDirty || finalMousePosition != initialMousePosition) {
                this->notifyMouseChanged(display);
                mouseRefreshUs = WebXController::MOUSE_MIN_REFRESH_DELAY_US;
            }

            std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double, std::micro> durationUs = end - start;
            long duration = durationUs.count();
            calculateThreadSleepUs = duration > this->_threadSleepUs ? 0 : this->_threadSleepUs - duration;

            float fps = 1000000 / delayUs.count();
            this->_stats.updateFrameData(fps, 0.001 * duration, imageSizeKB);
        }
    }

    spdlog::info("Stopped WebX Controller");
}

void WebXController::handleClientInstructions(WebXDisplay * display) {
    std::lock_guard<std::mutex> lock(this->_instructionsMutex);

    for (auto it = this->_instructions.begin(); it != this->_instructions.end(); it++) {
        auto instruction = *it;
        if (instruction->type == WebXInstruction::Type::Mouse) {
            auto mouseInstruction = std::static_pointer_cast<WebXMouseInstruction>(instruction);
            display->sendClientMouseInstruction(mouseInstruction->x, mouseInstruction->y, mouseInstruction->buttonMask);
        
        } else if (instruction->type == WebXInstruction::Type::Keyboard) {
           auto keyboardInstruction = std::static_pointer_cast<WebXKeyboardInstruction>(instruction);
            display->sendKeyboard(keyboardInstruction->key, keyboardInstruction->pressed);
    
        } else if (instruction->type == WebXInstruction::Type::Screen) {
            auto message = std::make_shared<WebXScreenMessage>(display->getScreenSize());
            this->sendMessage(message, instruction->id);

        } else if (instruction->type == WebXInstruction::Type::Windows) {
            auto message = std::make_shared<WebXWindowsMessage>(display->getVisibleWindowsProperties());
            this->sendMessage(message, instruction->id);
        
        } else if (instruction->type == WebXInstruction::Type::Image) {
            auto imageInstruction = std::static_pointer_cast<WebXImageInstruction>(instruction);
            // Client request full window image: make it the best quality 
            const WebXQuality & quality = WebXQuality::MaxQuality();
            std::shared_ptr<WebXImage> image = display->getImage(imageInstruction->windowId, quality);

            auto message = std::make_shared<WebXImageMessage>(imageInstruction->windowId, image);
            this->sendMessage(message, instruction->id);

        } else if (instruction->type == WebXInstruction::Type::Cursor) {
            auto cursorImageInstruction = std::static_pointer_cast<WebXCursorImageInstruction>(instruction);

            WebXMouse * mouse = display->getMouse();
            WebXMouseState * mouseState = mouse->getState();
            std::shared_ptr<WebXMouseCursor> mouseCursor = mouse->getCursor(cursorImageInstruction->cursorId);
            
            auto message = std::make_shared<WebXCursorImageMessage>(mouseState->getX(), mouseState->getY(), mouseCursor->getXhot(), mouseCursor->getYhot(), mouseCursor->getId(), mouseCursor->getImage());
            this->sendMessage(message, instruction->id);

        } else if (instruction->type == WebXInstruction::Type::Quality) {
            auto qualityInstruction = std::static_pointer_cast<WebXQualityInstruction>(instruction);
            uint32_t qualityIndex = qualityInstruction->qualityIndex;
            this->setRequestedQuality(qualityIndex);
        }
    }

    this->_instructions.clear();
}

void WebXController::notifyDisplayChanged(WebXDisplay * display) {
    this->_displayDirty = false;

    auto message = std::make_shared<WebXWindowsMessage>(display->getVisibleWindowsProperties());
    this->sendMessage(message);
}

float WebXController::notifyImagesChanged(WebXDisplay * display) {
    std::vector<WebXWindowDamageProperties> damagedWindows = display->getDamagedWindows(this->_requestedQuality);
    float imageSizeKB = 0.0;

    if (damagedWindows.size() > 0) {
        for (auto it = damagedWindows.begin(); it != damagedWindows.end(); it++) {
            WebXWindowDamageProperties & windowDamage = *it;

            if (windowDamage.isFullWindow() || windowDamage.getDamageAreaRatio() > 0.9) {
                WebXWindow * window = display->getVisibleWindow(windowDamage.windowId);
                if (window) {
                    // Get checksums before and after updating the window image
                    uint64_t oldChecksum = window->getWindowChecksum();
                    uint64_t oldAlphaChecksum = window->getWindowAlphaChecksum();
                    std::shared_ptr<WebXImage> image = display->getImage(windowDamage.windowId, this->_requestedQuality);
                    if (image) {
                        uint64_t newChecksum = window->getWindowChecksum();
                        uint64_t newAlphaChecksum = window->getWindowAlphaChecksum();

                        // Send event if checksum has changed
                        if (newChecksum != oldChecksum) {

                            // Compare alpha checksums
                            if (newAlphaChecksum == oldAlphaChecksum) {
                                bool removed = image->removeAlpha();
                                if (removed) {
                                    spdlog::trace("Removed alpha from image for window 0x{:01x}", windowDamage.windowId);
                                }
                            }

                            spdlog::trace("Window 0x{:x} sending encoded image {:d} x {:d} x {:d} @ {:d}KB (rgb = {:d}KB alpha = {:d}KB in {:d}ms)", windowDamage.windowId, image->getWidth(), image->getHeight(), image->getDepth(), (int)((1.0 * image->getFullDataSize()) / 1024), (int)((1.0 * image->getRawDataSize()) / 1024), (int)((1.0 * image->getAlphaDataSize()) / 1024), (int)(image->getEncodingTimeUs() / 1000));

                            auto message = std::make_shared<WebXImageMessage>(windowDamage.windowId, image);
                            this->sendMessage(message);

                            // Update stats
                            float imageSizeKB = image->getFullDataSize() / 1024.0;
                            display->onImageDataSent(windowDamage.windowId, imageSizeKB);
                            imageSizeKB += imageSizeKB;
                        }
                    }
                }

            } else {
                // Get sub image changes
                std::vector<WebXSubImage> subImages;
                for (auto it = windowDamage.damageAreas.begin(); it != windowDamage.damageAreas.end(); it++) {
                    WebXRectangle & area = *it;
                    std::shared_ptr<WebXImage> image = display->getImage(windowDamage.windowId, this->_requestedQuality, &area);
                    // Check image not null
                    if (image) {
                        subImages.push_back(WebXSubImage(area, image));

                        // Update stats
                        float imageSizeKB = image->getFullDataSize() / 1024.0;
                        display->onImageDataSent(windowDamage.windowId, imageSizeKB);
                        imageSizeKB += imageSizeKB;
                    }
                }

                if (subImages.size() > 0) {
                    for (auto it = subImages.begin(); it != subImages.end(); it++) {
                        const WebXSubImage & subImage = *it;
                        spdlog::trace("Window 0x{:x} sending encoded subimage {:d} x {:d} x {:d} @ {:d}KB (rgb = {:d}KB alpha = {:d}KB in {:d}ms)", windowDamage.windowId, subImage.imageRectangle.size.width, subImage.imageRectangle.size.height, subImage.image->getDepth(), (int)((1.0 * subImage.image->getFullDataSize()) / 1024), (int)((1.0 * subImage.image->getRawDataSize()) / 1024), (int)((1.0 * subImage.image->getAlphaDataSize()) / 1024), (int)(subImage.image->getEncodingTimeUs() / 1000));
                    }
    
                    auto message = std::make_shared<WebXSubImagesMessage>(windowDamage.windowId, subImages);
                    this->sendMessage(message);
                }
            }
        }
    }

    return imageSizeKB;
}

void WebXController::notifyMouseChanged(WebXDisplay * display) {
    this->_cursorDirty = false;

    const WebXMouseState  * mouseState = display->getMouse()->getState();

    auto message = std::make_shared<WebXMouseMessage>( mouseState->getX(), mouseState->getY(), mouseState->getCursor()->getId());
    this->sendMessage(message);
}


void WebXController::sendMessage(std::shared_ptr<WebXMessage> message, uint32_t commandId) {
    message->commandId = commandId;
    this->_gateway.publishMessage(message);
}

