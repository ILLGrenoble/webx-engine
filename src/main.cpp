#include <display/WebXManager.h>
#include <display/WebXController.h>
#include <connector/WebXClientConnector.h>
#include <connector/WebXKeyboardConnection.h>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"

int main(int argc, char *argv[]) {  
    spdlog::set_level(spdlog::level::info);
    
    int opt;
    while((opt = getopt(argc, argv, "l:")) != -1)  
    {  
        switch(opt)  
        {  
            case 'l': 
                if(strcmp(optarg,"debug") == 0) {
                    spdlog::set_level(spdlog::level::debug);
                } else if(strcmp(optarg, "trace") == 0) {
                    spdlog::set_level(spdlog::level::trace);
                }
                break;
        }  
    }  
      
    spdlog::info("Starting WebX");

    // WebXKeyboardConnection * keyboardConnection = new WebXKeyboardConnection();
    // keyboardConnection->run();

    // blocking
    WebXClientConnector::initInstance()->run();

    // delete keyboardConnection;
    spdlog::info("WebX terminated");
}
