#include "WebXController.h"
#include "WebXDisplay.h"
#include "WebXManager.h"
#include "WebXConnection.h"
#include <image/WebXSubImage.h>
#include <algorithm>
#include <thread>

unsigned int WebXController::THREAD_RATE = 120;
unsigned int WebXController::DISPLAY_REFRESH_RATE = 60;
unsigned int WebXController::IMAGE_REFRESH_RATE = 30;

WebXController::WebXController(WebXDisplay * display) :
    _display(display),
    _displayDirty(true),
    _displayRefreshUs(1000000.0 / WebXController::DISPLAY_REFRESH_RATE),
    _imageRefreshUs(1000000.0 / WebXController::IMAGE_REFRESH_RATE),
    _lastDisplayRefreshTime(std::chrono::high_resolution_clock::now()),
    _thread(NULL),
    _threadSleepUs(1000000.0 / WebXController::THREAD_RATE),
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

            // Update necessary images
            this->updateImages();
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

void WebXController::updateImages() {
    std::vector<WebXWindowDamageProperties> damagedWindows = this->_display->getDamagedWindows(this->_imageRefreshUs);
    if (damagedWindows.size() > 0) {
        WebXManager::instance()->pauseEventListener();

        // std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();

        for (auto it = damagedWindows.begin(); it != damagedWindows.end(); it++) {
            WebXWindowDamageProperties & windowDamage = *it;

            // printf("Damaged window 0x%0lx [%d x %d]:\n", windowDamage.windowId, windowDamage.windowSize.width, windowDamage.windowSize.height);
            for (auto it = windowDamage.damageAreas.begin(); it != windowDamage.damageAreas.end(); it++) {
                const WebXRectangle & rectangle = *it;
                // printf("Rectangle [%d %d %d %d]\n", rectangle.x, rectangle.y, rectangle.size.width, rectangle.size.height);
            }

            if (windowDamage.isFullWindow()) {
                // Get checksums before and after updating the window image
                uint64_t oldChecksum = this->_display->getWindowChecksum(windowDamage.windowId);
                std::shared_ptr<WebXImage> image = this->_display->getImage(windowDamage.windowId);
                if (image) {
                    uint64_t newChecksum = this->_display->getWindowChecksum(windowDamage.windowId);

                    // Send event if checksum has changed
                    if (newChecksum != oldChecksum) {
                        tthread::lock_guard<tthread::mutex> connectionsLock(this->_connectionsMutex);
                        printf("Sending image event for window 0x%0lx", windowDamage.windowId);
                        if (image->getRawDataSize() > 1024) {
                            printf(" [%d x % d x %d @ %dKB (%dms)]\n", image->getWidth(), image->getHeight(), image->getDepth(), (int)((1.0 * image->getRawDataSize()) / 1024), (int)(image->getEncodingTimeUs() / 1000));
                        } else {
                            printf(" [%d x % d x %d @ %fKB (%dus)]\n", image->getWidth(), image->getHeight(), image->getDepth(), (1.0 * image->getRawDataSize()) / 1024, (int)(image->getEncodingTimeUs()));
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
                printf("Sending subimage event for window 0x%0lx: [", windowDamage.windowId);
                for (auto it = subImages.begin(); it != subImages.end(); it++) {
                    const WebXSubImage & subImage = *it;
                    if (subImage.image->getRawDataSize() > 1024) {
                        printf(" [%d x % d x %d @ %dKB (%dms)]", subImage.imageRectangle.size.width, subImage.imageRectangle.size.height, subImage.image->getDepth(), (int)((1.0 * subImage.image->getRawDataSize()) / 1024), (int)(subImage.image->getEncodingTimeUs() / 1000));
                    } else {
                        printf(" [%d x % d x %d @ %fKB (%dus)]", subImage.imageRectangle.size.width, subImage.imageRectangle.size.height, subImage.image->getDepth(), (1.0 * subImage.image->getRawDataSize()) / 1024, (int)(subImage.image->getEncodingTimeUs()));
                    }
                } 
                printf(" ]\n");

                for (WebXConnection * connection : this->_connections) {
                    connection->onSubImagesChanged(windowDamage.windowId, subImages);
                }
            }

        }

        // std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
        // std::chrono::duration<double, std::micro> duration = end - start;
        // printf("updateImages took %fus\n", duration.count());

        WebXManager::instance()->resumeEventListener();
    }

}


