#include <display/WebXManager.h>
#include <controller/WebXController.h>
#include <gateway/WebXGateway.h>
#include <transport/WebXTransport.h>
#include <display/WebXDisplay.h>
#include <models/WebXSettings.h>
#include <spdlog/spdlog.h>
#include <string>
#include <csignal>

static WebXController * controller = nullptr;

/**
 * @brief Signal handler for handling system signals.
 * 
 * This function handles the SIGINT signal to gracefully shut down the application.
 * It stops the WebXController if it is running.
 * 
 * @param signal The signal number received.
 */
static void signalHandler(int signal) {
    if (signal == SIGINT) {
        spdlog::info("Shutdown");

        if (controller) {
            controller->stop();
        }
    }
}

/**
 * @brief Entry point of the WebX application.
 * 
 * This function initializes the application, processes command-line arguments,
 * sets up the necessary components (controller, transport, etc.), and starts
 * the WebX server. It also handles graceful shutdown on receiving SIGINT.
 * 
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @return int Exit status of the application.
 */
int main(int argc, char *argv[]) {  
    spdlog::set_level(spdlog::level::info);
    std::signal(SIGINT, signalHandler);

    WebXSettings settings;
    if (settings.logLevel == "debug") {
        spdlog::set_level(spdlog::level::debug);
    
    } else if (settings.logLevel == "trace") {
        spdlog::set_level(spdlog::level::trace);
    }

    int opt;
    std::string keyboardLayout = "";
    bool standAlone = false;
    bool testing = false;

    while((opt = getopt(argc, argv, "k:st")) != -1) {  
        switch(opt)  
        {  
            case 's':
                standAlone = true;
                spdlog::info("Starting WebX in stand-alone mode");
                break;

            case 'k':
                keyboardLayout = optarg;
                break;

            case 't':
                testing = true;
                spdlog::info("Starting WebX in test mode");
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
    if (!testing) {
        transport.start();
    }

    // Start the controller (blocking)
    controller->run(testing);

    // stop transport
    transport.stop();

    delete controller;
    controller = nullptr;

    spdlog::info("WebX terminated");
}
