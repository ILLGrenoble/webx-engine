#include <display/WebXManager.h>
#include <controller/WebXController.h>
#include <gateway/WebXGateway.h>
#include <transport/WebXTransport.h>
#include <display/WebXDisplay.h>
#include <utils/WebXSettings.h>
#include <spdlog/spdlog.h>
#include <string>
#include <csignal>

static WebXController * controller = nullptr;

static void signalHandler(int signal) {
    if (signal == SIGINT) {
        spdlog::info("Shutdown");

        if (controller) {
            controller->stop();
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

    // Create the Gateway (connection between transport layer and controller)
    WebXGateway gateway;

    // Initialise Manager, Display and Event Listener
    controller = new WebXController(gateway, settings, keyboardLayout);

    // Start transport
    WebXTransport transport(gateway, settings.transport, standAlone);
    transport.start();

    // Start the controller (blocking)
    controller->run();

    // stop transport
    transport.stop();

    delete controller;
    controller = nullptr;

    spdlog::info("WebX terminated");
}
