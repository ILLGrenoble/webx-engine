#include <display/WebXManager.h>
#include <display/WebXController.h>
#include <display/WebXDisplay.h>
#include <connector/WebXClientConnector.h>
#include <connector/WebXKeyboardConnection.h>
#include <spdlog/spdlog.h>
#include "spdlog/sinks/stdout_color_sinks.h"
#include <string>

int main(int argc, char *argv[]) {  
    spdlog::set_level(spdlog::level::info);
    
    int opt;
    bool useKeyboard = false;
    int socketTimeoutMs = -1;

    while((opt = getopt(argc, argv, "l:ik:")) != -1) {  
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
                    WebXKeyboardConnection * keyboardConnection = new WebXKeyboardConnection();
                    keyboardConnection->run();
                    spdlog::info("Initialised WebX interactive keyboard connection");
                    socketTimeoutMs = 1000;
                }
                break;

            case 'k':
                {
                    std::string layout = optarg;
                    if (!layout.empty()) {
                        WebXDisplay * webXDisplay = WebXManager::instance()->getDisplay();
                        if (webXDisplay->loadKeyboardLayout(layout)) {
                            spdlog::info("Loaded '{:s}' keyboard layout", layout);

                        } else {
                            spdlog::error("Failed to load '{:s}' keyboard layout", layout);
                        }
                    }
                }
                break;

        }  
    }  
      
    // blocking
    spdlog::info("Starting WebX server");
    WebXClientConnector::initInstance()->run(socketTimeoutMs);

    // delete keyboardConnection;
    spdlog::info("WebX terminated");
}
