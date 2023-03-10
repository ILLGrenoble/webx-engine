#include "WebXKeyboardConnection.h"
#include <controller/WebXController.h>
#include <display/WebXManager.h>
#include <display/WebXDisplay.h>
#include <display/WebXWindow.h>
#include <events/WebXEventListener.h>
#include <image/WebXImage.h>
#include <fort/fort.h>
#include <stdlib.h>
#include <iostream>
#include <spdlog/spdlog.h>

WebXKeyboardConnection::WebXKeyboardConnection() : 
    _thread(NULL),
    _running(false) {
}

WebXKeyboardConnection::~WebXKeyboardConnection() {
    this->stop();
}

void WebXKeyboardConnection::run() {
    std::lock_guard<std::mutex> lock(this->_mutex);
    this->_running = true;
    if (this->_thread == NULL) {
        this->_thread = new std::thread(&WebXKeyboardConnection::mainLoop, this);
    }
}

void WebXKeyboardConnection::stop() {
    std::lock_guard<std::mutex> lock(this->_mutex);
    this->_running = false;
    if (this->_thread != NULL) {
        // Join thread and cleanup
        spdlog::info("Stopping keyboard connector...");
        this->_thread->join();
        spdlog::info("Stopped keyboard connector");
        delete this->_thread;
        this->_thread = NULL;
    }
}

void WebXKeyboardConnection::mainLoop() {
    while (this->_running) {
        char key;
        std::cin >> key;
        if (key == 'p') {
            this->printWindows();

            this->exportWindowImages();
        
        } else if (key == 'q') {
            this->_running = false;
            WebXController::shutdown();
        }
    }
}

void WebXKeyboardConnection::printWindows() {
    ft_table_t * table = ft_create_table();
    ft_set_cell_prop(table, 0, FT_ANY_COLUMN, FT_CPROP_ROW_TYPE, FT_ROW_HEADER);
    ft_write_ln(table, "ID", "Managed ID", "x", "y", "Width", "Height");
    WebXManager * manager = WebXController::instance()->getManager();

    const std::vector<WebXWindow *> windows = manager->getDisplay()->getVisibleWindows();
    for (std::vector<WebXWindow *>::const_iterator it = windows.begin(); it != windows.end(); it++) {
        WebXWindow * window = *it;

        WebXWindow * managedWindow = manager->getDisplay()->getManagedWindow(window);

        const WebXRectangle & rectangle = window->getTopParent()->getRectangle();
        if (managedWindow != NULL) {
            ft_printf_ln(table, "0x%08lx|0x%08lx|%d|%d|%d|%d", window->getX11Window(), managedWindow->getX11Window(), rectangle.x, rectangle.y, rectangle.size.width, rectangle.size.height);

        } else {
            ft_printf_ln(table, "0x%08lx| |%d|%d|%d|%d", window->getX11Window(), rectangle.x, rectangle.y, rectangle.size.width, rectangle.size.height);
        }
    }

    printf("%s\n", ft_to_string(table));
    ft_destroy_table(table);
}

void WebXKeyboardConnection::exportWindowImages() {
    WebXManager * manager = WebXController::instance()->getManager();

    const std::vector<WebXWindow *> windows = manager->getDisplay()->getVisibleWindows();
    for (std::vector<WebXWindow *>::const_iterator it = windows.begin(); it != windows.end(); it++) {
        WebXWindow * window = (*it);

        std::shared_ptr<WebXImage> image = manager->getDisplay()->getImage(window->getX11Window());
        if (image) {
            char filename[64];
            snprintf(filename, sizeof(filename) - 1, "images/window-0x%08lx", window->getX11Window());
            
            if (!image->save(filename)) {
                printf("Failed to write image for window 0X%08lx\n", window->getX11Window());
            }
        }
    }
}

