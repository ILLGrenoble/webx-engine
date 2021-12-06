#include <display/WebXManager.h>
#include <controller/WebXController.h>
#include <controller/WebXKeyboardConnection.h>
#include <transport/WebXTransport.h>
#include <display/WebXDisplay.h>
#include <spdlog/spdlog.h>
#include "spdlog/sinks/stdout_color_sinks.h"
#include <string>
#include <csignal>

static void signalHandler(int signal) {
    if (signal == SIGINT) {
        spdlog::info("Shutdown");

        WebXController::instance()->stop();
    }
}

void setKeyboardLayout(const std::string & keyboardLayout) {
    if (!keyboardLayout.empty()) {
        WebXDisplay * webXDisplay = WebXController::instance()->getManager()->getDisplay();
        if (webXDisplay->loadKeyboardLayout(keyboardLayout)) {
            spdlog::info("Loaded '{:s}' keyboard layout", keyboardLayout);

        } else {
            spdlog::error("Failed to load '{:s}' keyboard layout", keyboardLayout);
        }
    }
}

int main(int argc, char *argv[]) {  
    spdlog::set_level(spdlog::level::info);
    std::signal(SIGINT, signalHandler);

    int opt;
    WebXKeyboardConnection * keyboardConnection = NULL;
    bool useKeyboard = false;
    int socketTimeoutMs = -1;
    std::string keyboardLayout = "";
    bool standAlone = false;

    while((opt = getopt(argc, argv, "l:ik:s")) != -1) {  
        switch(opt)  
        {  
            case 'l': 
                if (strcmp(optarg, "debug") == 0) {
                    spdlog::set_level(spdlog::level::debug);
                } else if (strcmp(optarg, "trace") == 0) {
                    spdlog::set_level(spdlog::level::trace);
                }
                break;
             
            case 'i':
                {
                    keyboardConnection = new WebXKeyboardConnection();
                    keyboardConnection->run();
                    spdlog::info("Initialised WebX interactive keyboard connection");
                }
                break;
             
            case 's':
                {
                    standAlone = true;
                    spdlog::info("Starting WebX in Stand Alone mode");
                }
                break;

            case 'k':
                keyboardLayout = optarg;
                break;

        }  
    }  
      
    spdlog::info("Starting WebX server");

    // Initialise Manager, Display and Event Listener
    WebXController::instance()->init();

    // Set keyboard layout
    setKeyboardLayout(keyboardLayout);

    // Start transport
    WebXTransport * transport = new WebXTransport(standAlone);
    transport->start();

    // Start the controller (blocking)
    WebXController::instance()->run();

    // stop transport
    transport->stop();

    if (keyboardConnection != NULL) {
        keyboardConnection->stop();
    } 

    spdlog::info("WebX terminated");
}
