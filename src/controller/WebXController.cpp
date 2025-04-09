#include "WebXController.h"
#include <display/WebXDisplay.h>
#include <models/instruction/WebXMouseInstruction.h>
#include <models/instruction/WebXKeyboardInstruction.h>
#include <models/instruction/WebXImageInstruction.h>
#include <models/instruction/WebXCursorImageInstruction.h>
#include <models/instruction/WebXQualityInstruction.h>
#include <models/instruction/WebXPongInstruction.h>
#include <models/instruction/WebXDataAckInstruction.h>
#include <models/instruction/WebXClipboardInstruction.h>
#include <models/message/WebXScreenMessage.h>
#include <models/message/WebXWindowsMessage.h>
#include <models/message/WebXImageMessage.h>
#include <models/message/WebXCursorImageMessage.h>
#include <models/message/WebXSubImagesMessage.h>
#include <models/message/WebXMouseMessage.h>
#include <image/WebXSubImage.h>
#include <display/input/WebXMouse.h>
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
    _clientRegistry(settings, [&gateway](std::shared_ptr<WebXMessage> message) {
        gateway.publishMessage(message);
    }),
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
    this->_manager.setClipboardEventHandler([this](const std::string clipboardContent) { this->onClipboardEvent(clipboardContent); });
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

            // Check if the mouse has been moved by some other application (or locally)
            if (timeSinceMouseRefreshUs.count() > MOUSE_REFRESH_DELAY_US) {
                mouse->updatePosition();
                if (mouse->isDirty()) {
                    this->notifyMouseChanged(mouse);
                    mouse->setDirty(false);
                    lastMouseRefreshTime = std::chrono::high_resolution_clock::now();
                }
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
            this->onClientMouseInstruction(display, mouseInstruction, client);
        
        } else if (instruction->type == WebXInstruction::Type::Keyboard) {
           auto keyboardInstruction = std::static_pointer_cast<WebXKeyboardInstruction>(instruction);
            display->sendKeyboard(keyboardInstruction->key, keyboardInstruction->pressed);
    
        } else if (instruction->type == WebXInstruction::Type::Screen) {
            // Send message to specific client
            auto message = std::make_shared<WebXScreenMessage>(client->getIndex(), instruction->id, display->getScreenSize());
            this->sendMessage(message);

        } else if (instruction->type == WebXInstruction::Type::Windows) {
            // Send message to specific client
            auto message = std::make_shared<WebXWindowsMessage>(client->getIndex(), instruction->id, display->getVisibleWindowsProperties());
            this->sendMessage(message);
        
        } else if (instruction->type == WebXInstruction::Type::Image) {
            auto imageInstruction = std::static_pointer_cast<WebXImageInstruction>(instruction);
            // Client request full window image: make it the best quality 
            const WebXQuality & quality = WebXQuality::MaxQuality();
            std::shared_ptr<WebXImage> image = display->getImage(imageInstruction->windowId, quality);

            // Send message to specific client
            auto message = std::make_shared<WebXImageMessage>(client->getIndex(), instruction->id, imageInstruction->windowId, image);
            this->sendMessage(message);

        } else if (instruction->type == WebXInstruction::Type::Cursor) {
            auto cursorImageInstruction = std::static_pointer_cast<WebXCursorImageInstruction>(instruction);

            WebXMouse * mouse = display->getMouse();
            WebXMouseState * mouseState = mouse->getState();
            std::shared_ptr<WebXMouseCursor> mouseCursor = mouse->getCursor(cursorImageInstruction->cursorId);
            
            // Send message to specific client
            auto message = std::make_shared<WebXCursorImageMessage>(client->getIndex(), instruction->id, mouseState->getX(), mouseState->getY(), mouseCursor->getXhot(), mouseCursor->getYhot(), mouseCursor->getId(), mouseCursor->getImage());
            this->sendMessage(message);

        } else if (instruction->type == WebXInstruction::Type::Quality) {
            auto qualityInstruction = std::static_pointer_cast<WebXQualityInstruction>(instruction);
            uint32_t qualityIndex = qualityInstruction->qualityIndex;
            this->_clientRegistry.setClientMaxQuality(qualityInstruction->clientId, WebXQuality::QualityForIndex(qualityIndex));

        } else if (instruction->type == WebXInstruction::Type::Pong) {
            auto pongInstruction = std::static_pointer_cast<WebXPongInstruction>(instruction);
            this->_clientRegistry.onPongReceived(pongInstruction->clientId, pongInstruction->sendTimestampMs, pongInstruction->recvTimestampMs);

        } else if (instruction->type == WebXInstruction::Type::DataAck) {
            auto dataAckInstruction = std::static_pointer_cast<WebXDataAckInstruction>(instruction);
            this->_clientRegistry.onDataAckReceived(dataAckInstruction->clientId, dataAckInstruction->sendTimestampMs, dataAckInstruction->recvTimestampMs, dataAckInstruction->dataLength);

        } else if (instruction->type == WebXInstruction::Type::Clipboard) {
            auto clipboardInstruction = std::static_pointer_cast<WebXClipboardInstruction>(instruction);
            this->_manager.setClipboardContent(clipboardInstruction->clipboardContent);
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
    this->_clientRegistry.handleClientPings();
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

            WebXController::WebXImageUpdateVerification verification = this->verifyImageUpdate(image, window);
            if (verification.hasChanged) {
                spdlog::trace("Window 0x{:x} sending encoded image {:d} x {:d} x {:d} @ {:d}KB (rgb = {:d}KB alpha = {:d}KB in {:d}ms)", window->getId(), image->getWidth(), image->getHeight(), image->getDepth(), (int)((1.0 * image->getFullDataSize()) / 1024), (int)((1.0 * image->getRawDataSize()) / 1024), (int)((1.0 * image->getAlphaDataSize()) / 1024), (int)(image->getEncodingTimeUs() / 1000));

                // Send message group of clients for the window full image update
                this->sendMessage(std::make_shared<WebXImageMessage>(clientIndexMask, window->getId(), image));

                // Update stats
                float imageSizeKB = image->getFullDataSize() / 1024.0;
                totalImageSizeKB += imageSizeKB;

                // Return full window transfer data
                return WebXResult<WebXWindowImageTransferData>::Ok(WebXWindowImageTransferData(window->getId(), imageSizeKB, verification.rgbChecksum, verification.alphaChecksum));
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

    // Verify quality settings for each client
    this->_clientRegistry.performQualityVerification();

    return totalImageSizeKB;
}

void WebXController::onClientMouseInstruction(WebXDisplay * display, const std::shared_ptr<WebXMouseInstruction> & mouseInstruction, const std::shared_ptr<WebXClient> & client) {
    const WebXMouse * mouse = display->getMouse();
    const WebXMouseState * mouseState = mouse->getState();
    WebXPosition previousMousePosition(mouseState->getX(), mouseState->getY());
    
    // Update the mouse position and button mask
    display->sendClientMouseInstruction(mouseInstruction->x, mouseInstruction->y, mouseInstruction->buttonMask);

    // If position has change notify the clients (except the one sending the instruction)
    if (previousMousePosition.x() != mouseInstruction->x || previousMousePosition.y() != mouseInstruction->y) {
        // Set bitmask to all clients except the one sending the instruction (if it exists)
        uint64_t clientIndexMask = client ? ~client->getIndex() : GLOBAL_CLIENT_INDEX_MASK;

        // Send message to all clients (other that the one sending the instruction)
        auto message = std::make_shared<WebXMouseMessage>(clientIndexMask, mouseInstruction->x, mouseInstruction->y, mouse->getState()->getCursor()->getId());
        this->sendMessage(message);
    }
}

void WebXController::notifyMouseChanged(WebXMouse * mouse) {
    this->_cursorDirty = false;

    const WebXMouseState * mouseState = mouse->getState();

    // Send message to all clients
    auto message = std::make_shared<WebXMouseMessage>(GLOBAL_CLIENT_INDEX_MASK, mouseState->getX(), mouseState->getY(), mouseState->getCursor()->getId());
    this->sendMessage(message);
}

WebXController::WebXImageUpdateVerification WebXController::verifyImageUpdate(std::shared_ptr<WebXImage> & image, const std::unique_ptr<WebXClientWindow> & window) {
    // Verify that the image is not null
    if (image == nullptr) {
        return WebXImageUpdateVerification{0, 0, false};
    }

    if (this->_settings.controller.imageCheckumEnabled) {
        // Calculate the image checksums
        uint32_t rgbChecksum = image->calculateImageChecksum();
        uint32_t alphaChecksum = image->calculateAlphaChecksum();

        // Send event if checksum has changed
        if (rgbChecksum != window->getRGBChecksum()) {

            // Compare alpha checksums
            if (alphaChecksum == window->getAlphaChecksum()) {
                bool removed = image->removeAlpha();
                if (removed) {
                    spdlog::trace("Removed alpha from image for window 0x{:01x}", window->getId());
                }
            }

            return WebXImageUpdateVerification{rgbChecksum, alphaChecksum, true};
        }

        return WebXImageUpdateVerification{rgbChecksum, alphaChecksum, false};
    
    } else {
        // No checksum verification: always return true
        return WebXImageUpdateVerification{0, 0, true};
    }
}

