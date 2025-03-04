#include "WebXController.h"
#include <display/WebXDisplay.h>
#include <instruction/WebXMouseInstruction.h>
#include <instruction/WebXKeyboardInstruction.h>
#include <instruction/WebXImageInstruction.h>
#include <instruction/WebXCursorImageInstruction.h>
#include <instruction/WebXQualityInstruction.h>
#include <instruction/WebXPongInstruction.h>
#include <message/WebXScreenMessage.h>
#include <message/WebXWindowsMessage.h>
#include <message/WebXImageMessage.h>
#include <message/WebXCursorImageMessage.h>
#include <message/WebXSubImagesMessage.h>
#include <message/WebXMouseMessage.h>
#include <message/WebXPingMessage.h>
#include <image/WebXSubImage.h>
#include <input/WebXMouse.h>
#include <utils/WebXResult.h>
#include <models/WebXQuality.h>
#include <models/WebXPosition.h>
#include <algorithm>
#include <thread>
#include <spdlog/spdlog.h>

const uint64_t WebXController::GLOBAL_CLIENT_INDEX_MASK = ~0x00; // Sets all bits

WebXController::WebXController(WebXGateway & gateway, const WebXSettings & settings, const std::string & keyboardLayout) :
    _gateway(gateway),
    _settings(settings),
    _manager(settings, keyboardLayout),
    _clientRegistry(settings),
    _displayDirty(true),
    _cursorDirty(true),
    _threadSleepUs(1000000.0 / WebXController::THREAD_RATE),
    _state(WebXControllerState::Stopped) {

    // Set the instruction handler function in the gateway
    this->_gateway.setInstructionHandlerFunc([this](std::shared_ptr<WebXInstruction> instruction) {
        const std::lock_guard<std::mutex> lock(this->_instructionsMutex);
        this->_instructions.push_back(instruction);
    });

    // Set the client registry functions in the gateway
    this->_gateway.setClientConnectFunc([this]() { return this->_clientRegistry.addClient(); });
    this->_gateway.setClientDisconnectFunc([this](uint32_t clientId) { return this->_clientRegistry.removeClient(clientId); });

    // Listen to events from the display
    this->_manager.setDisplayEventHandler([this](WebXDisplayEventType eventType) { this->onDisplayEvent(eventType); });
    this->_manager.setDamageEventHandler([this](const WebXWindowDamage damage) { this->_clientRegistry.addWindowDamage(damage); });
}

WebXController::~WebXController() {
}

void WebXController::stop() {
    spdlog::info("Shutdown");
    this->_state = WebXControllerState::Stopped;

    // Remove the instruction handler
    this->_gateway.setInstructionHandlerFunc(nullptr);

    // Remove the client registry functions
    this->_gateway.setClientConnectFunc(nullptr);
    this->_gateway.setClientDisconnectFunc(nullptr);
    
    // Remove the display events listener
    this->_manager.setDisplayEventHandler(nullptr);
    this->_manager.setDamageEventHandler(nullptr);
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

            // Handle client pings
            this->handleClientPings();

            // Update necessary images of the client windows
            float imageSizeKB = this->updateClientWindows(display);

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

    for (const auto & instruction : this->_instructions) {

        // Verify that the instruction->clientId is known. Reject the instruction if client unknown
        const std::shared_ptr<WebXClient> & client = this->_clientRegistry.getClientById(instruction->clientId);
        if (client == nullptr) {
            spdlog::debug("Ignoring instruction {:d} from unknown client {:08x}", (int)instruction->type, instruction->clientId);
            continue;
        }

        if (instruction->type == WebXInstruction::Type::Mouse) {
            auto mouseInstruction = std::static_pointer_cast<WebXMouseInstruction>(instruction);
            display->sendClientMouseInstruction(mouseInstruction->x, mouseInstruction->y, mouseInstruction->buttonMask);
        
        } else if (instruction->type == WebXInstruction::Type::Keyboard) {
           auto keyboardInstruction = std::static_pointer_cast<WebXKeyboardInstruction>(instruction);
            display->sendKeyboard(keyboardInstruction->key, keyboardInstruction->pressed);
    
        } else if (instruction->type == WebXInstruction::Type::Screen) {
            // Send message to specific client
            auto message = std::make_shared<WebXScreenMessage>(client->getIndex(), display->getScreenSize());
            this->sendMessage(message, instruction->id);

        } else if (instruction->type == WebXInstruction::Type::Windows) {
            // Send message to specific client
            auto message = std::make_shared<WebXWindowsMessage>(client->getIndex(), display->getVisibleWindowsProperties());
            this->sendMessage(message, instruction->id);
        
        } else if (instruction->type == WebXInstruction::Type::Image) {
            auto imageInstruction = std::static_pointer_cast<WebXImageInstruction>(instruction);
            // Client request full window image: make it the best quality 
            const WebXQuality & quality = WebXQuality::MaxQuality();
            std::shared_ptr<WebXImage> image = display->getImage(imageInstruction->windowId, quality);

            // Send message to specific client
            auto message = std::make_shared<WebXImageMessage>(client->getIndex(), imageInstruction->windowId, image);
            this->sendMessage(message, instruction->id);

        } else if (instruction->type == WebXInstruction::Type::Cursor) {
            auto cursorImageInstruction = std::static_pointer_cast<WebXCursorImageInstruction>(instruction);

            WebXMouse * mouse = display->getMouse();
            WebXMouseState * mouseState = mouse->getState();
            std::shared_ptr<WebXMouseCursor> mouseCursor = mouse->getCursor(cursorImageInstruction->cursorId);
            
            // Send message to specific client
            auto message = std::make_shared<WebXCursorImageMessage>(client->getIndex(), mouseState->getX(), mouseState->getY(), mouseCursor->getXhot(), mouseCursor->getYhot(), mouseCursor->getId(), mouseCursor->getImage());
            this->sendMessage(message, instruction->id);

        } else if (instruction->type == WebXInstruction::Type::Quality) {
            auto qualityInstruction = std::static_pointer_cast<WebXQualityInstruction>(instruction);
            uint32_t qualityIndex = qualityInstruction->qualityIndex;
            this->_clientRegistry.setClientQuality(qualityInstruction->clientId, WebXQuality::QualityForIndex(qualityIndex));

        } else if (instruction->type == WebXInstruction::Type::Pong) {
            auto pongInstruction = std::static_pointer_cast<WebXPongInstruction>(instruction);
            this->_clientRegistry.onPongReceived(pongInstruction->clientId);
        }
    }

    this->_instructions.clear();
}

void WebXController::notifyDisplayChanged(WebXDisplay * display) {
    this->_displayDirty = false;

    // Send message to all clients
    auto message = std::make_shared<WebXWindowsMessage>(GLOBAL_CLIENT_INDEX_MASK, display->getVisibleWindowsProperties());
    this->sendMessage(message);
}

void WebXController::handleClientPings() {
    this->_clientRegistry.handleClientPings([this](uint32_t clientIndex) {
        spdlog::trace("Sending Ping to client with index {:016x}", clientIndex);

        auto message = std::make_shared<WebXPingMessage>(clientIndex);
        this->sendMessage(message);
    });
}

float WebXController::updateClientWindows(WebXDisplay * display) {
    // Send all current window visibilities to registry to update all current visible client windows and their coverage
    this->_clientRegistry.updateVisibleWindows(display->getWindowVisiblities());

    // Handle all necessary damage in the client windows
    float totalImageSizeKB = 0.0;
    this->_clientRegistry.handleWindowDamage([&](const std::unique_ptr<WebXClientWindow> & window, uint64_t clientIndexMask) { 
        const WebXWindowDamage & windowDamage = window->getDamage();
        if (window->isFullWindowDamage() || window->getDamageAreaRatio() > 0.9) {
            std::shared_ptr<WebXImage> image = display->getImage(window->getId(), window->getCurrentQuality());
            if (image) {
                // Calculate the image checksums
                uint64_t rgbChecksum = image->calculateImageChecksum();
                uint64_t alphaChecksum = image->calculateAlphaChecksum();

                // Send event if checksum has changed
                if (rgbChecksum != window->getRGBChecksum()) {

                    // Compare alpha checksums
                    if (alphaChecksum == window->getAlphaChecksum()) {
                        bool removed = image->removeAlpha();
                        if (removed) {
                            spdlog::trace("Removed alpha from image for window 0x{:01x}", window->getId());
                        }
                    }

                    spdlog::trace("Window 0x{:x} sending encoded image {:d} x {:d} x {:d} @ {:d}KB (rgb = {:d}KB alpha = {:d}KB in {:d}ms)", window->getId(), image->getWidth(), image->getHeight(), image->getDepth(), (int)((1.0 * image->getFullDataSize()) / 1024), (int)((1.0 * image->getRawDataSize()) / 1024), (int)((1.0 * image->getAlphaDataSize()) / 1024), (int)(image->getEncodingTimeUs() / 1000));

                    // Send message group of clients for the window full image update
                    this->sendMessage(std::make_shared<WebXImageMessage>(clientIndexMask, window->getId(), image));

                    // Update stats
                    float imageSizeKB = image->getFullDataSize() / 1024.0;
                    totalImageSizeKB += imageSizeKB;

                    // Return full window transfer data
                    return WebXResult<WebXWindowImageTransferData>::Ok(WebXWindowImageTransferData(window->getId(), imageSizeKB, rgbChecksum, alphaChecksum));
                }
            }

        } else {
            // Get sub image changes
            std::vector<WebXSubImage> subImages;
            float totalSubImagesSizeKB = 0.0;
            for (const WebXRectangle & area: window->getDamage().getDamagedAreas()) {
                std::shared_ptr<WebXImage> image = display->getImage(window->getId(), window->getCurrentQuality(), &area);
                // Check image not null
                if (image) {
                    subImages.push_back(WebXSubImage(area, image));
                    totalSubImagesSizeKB += image->getFullDataSize() / 1024.0;
                }
            }

            if (subImages.size() > 0) {
                for (auto it = subImages.begin(); it != subImages.end(); it++) {
                    const WebXSubImage & subImage = *it;
                    spdlog::trace("Window 0x{:x} sending encoded subimage {:d} x {:d} x {:d} @ {:d}KB (rgb = {:d}KB alpha = {:d}KB in {:d}ms)", window->getId(), subImage.imageRectangle.size().width(), subImage.imageRectangle.size().height(), subImage.image->getDepth(), (int)((1.0 * subImage.image->getFullDataSize()) / 1024), (int)((1.0 * subImage.image->getRawDataSize()) / 1024), (int)((1.0 * subImage.image->getAlphaDataSize()) / 1024), (int)(subImage.image->getEncodingTimeUs() / 1000));
                }

                // Send message group of clients for the window sub-image updates
                this->sendMessage(std::make_shared<WebXSubImagesMessage>(clientIndexMask, window->getId(), subImages));

                // Update stats
                totalImageSizeKB += totalSubImagesSizeKB;

                // Return sub window transfer data
                return WebXResult<WebXWindowImageTransferData>::Ok(WebXWindowImageTransferData(window->getId(), totalSubImagesSizeKB));
            }            
        }

        // Return ignored window transfer data
        return WebXResult<WebXWindowImageTransferData>::Ok(WebXWindowImageTransferData(window->getId(), WebXWindowImageTransferData::WebXWindowImageTransferStatus::Ignored));
    });

    return totalImageSizeKB;
}

void WebXController::notifyMouseChanged(WebXDisplay * display) {
    this->_cursorDirty = false;

    const WebXMouseState  * mouseState = display->getMouse()->getState();

    // Send message to all clients
    auto message = std::make_shared<WebXMouseMessage>(GLOBAL_CLIENT_INDEX_MASK, mouseState->getX(), mouseState->getY(), mouseState->getCursor()->getId());
    this->sendMessage(message);
}


void WebXController::sendMessage(std::shared_ptr<WebXMessage> message, uint32_t commandId) {
    message->commandId = commandId;
    this->_gateway.publishMessage(message);
}

