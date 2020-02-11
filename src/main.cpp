#include <display/WebXManager.h>
#include <display/WebXController.h>
#include <connector/WebXClientConnector.h>
#include <connector/WebXKeyboardConnection.h>
#include <spdlog/spdlog.h>
#include "spdlog/sinks/stdout_color_sinks.h"
#include <string>

int main(int argc, char *argv[]) {  
    spdlog::set_level(spdlog::level::info);
    
    int opt;
    std::string transport = "binary";
    bool useKeyboard = false;
    int socketTimeoutMs = -1;

    while((opt = getopt(argc, argv, "l:t:k")) != -1) {  
        switch(opt)  
        {  
            case 'l': 
                if (strcmp(optarg,"debug") == 0) {
                    spdlog::set_level(spdlog::level::debug);
                } else if (strcmp(optarg, "trace") == 0) {
                    spdlog::set_level(spdlog::level::trace);
                }
                break;

            case 't':
                if (strcmp(optarg,"json") == 0) {
                    transport = optarg;
                } else {
                    spdlog::error("The provided transport '{}' is unknown. Defaulting to using binary transport", optarg);
                }
                break;

            case 'k':
                WebXKeyboardConnection * keyboardConnection = new WebXKeyboardConnection();
                keyboardConnection->run();
                spdlog::info("Initialised WebX keyboard connection");
                socketTimeoutMs = 1000;
                break;
        }  
    }  
      
    // blocking
    spdlog::info("Starting WebX server");
    WebXClientConnector::initInstance(transport)->run(socketTimeoutMs);

    // delete keyboardConnection;
    spdlog::info("WebX terminated");
}
