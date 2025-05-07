#include <display/WebXManager.h>
#include <controller/WebXController.h>
#include <gateway/WebXGateway.h>
#include <transport/WebXTransport.h>
#include <display/WebXDisplay.h>
#include <models/WebXSettings.h>
#include <version.h>
#include <spdlog/spdlog.h>
#include <string>
#include <csignal>
#include <iostream>
#include <getopt.h>


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


void printUsage() {
    std::cout << "Usage: webx-engine [options]\n"
              << "Options:\n"
              << "  -s, --standalone         Run in stand-alone mode\n"
              << "  -t, --test               Run in test mode\n"
              << "  -k, --keyboard LAYOUT    Set keyboard layout (e.g., 'gb')\n"
              << "      --version            Print version and exit\n"
              << "      --help               Show this help message\n";
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

    const struct option long_options[] = {
        {"keyboard", required_argument, nullptr, 'k'},
        {"standalone", no_argument, nullptr, 's'},
        {"test", no_argument, nullptr, 't'},
        {"version", no_argument, nullptr, 1},
        {"help", no_argument, nullptr, 2},
        {nullptr, 0, nullptr, 0}
    };

    while ((opt = getopt_long(argc, argv, "k:st", long_options, nullptr)) != -1) {
        switch (opt) {
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

            case 1: // --version
                std::cout << "webx-engine " << WEBX_ENGINE_VERSION << std::endl;
                return 0;

            case 2: // --help
                printUsage();
                return 0;
    
            default:
                std::cerr << "Unknown option.\n";
                printUsage();
                return 1;
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
