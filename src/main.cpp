#include <display/WebXManager.h>
#include <display/WebXController.h>
#include <connector/WebXClientConnector.h>
#include <connector/WebXKeyboardConnection.h>

int main() {
    // WebXKeyboardConnection * keyboardConnection = new WebXKeyboardConnection();
    // keyboardConnection->run();

    // blocking
    WebXClientConnector::initInstance()->run();

    // delete keyboardConnection;
    printf("WebX terminated\n");
}
