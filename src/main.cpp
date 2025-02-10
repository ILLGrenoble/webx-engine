#include <display/WebXManager.h>
#include <controller/WebXController.h>
#include <transport/WebXTransport.h>
#include <display/WebXDisplay.h>
#include <utils/WebXSettings.h>
#include <spdlog/spdlog.h>
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

    WebXSettings settings;
    if (settings.logging == "debug") {
        spdlog::set_level(spdlog::level::debug);
    
    } else if (settings.logging == "trace") {
        spdlog::set_level(spdlog::level::trace);
    }

    int opt;
    std::string keyboardLayout = "";
    bool standAlone = false;

    while((opt = getopt(argc, argv, "ik:s")) != -1) {  
        switch(opt)  
        {  
            case 's':
                {
                    standAlone = true;
                    spdlog::info("Starting WebX in stand-alone mode");
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
    WebXTransport * transport = new WebXTransport(&settings, standAlone);
    transport->start();

    // Start the controller (blocking)
    WebXController::instance()->run();

    // stop transport
    transport->stop();

    spdlog::info("WebX terminated");
}
