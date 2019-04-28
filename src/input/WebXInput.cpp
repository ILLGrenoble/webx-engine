#include "WebXInput.h"

WebXInput::WebXInput(WebXMouse *mouse): _mouse(mouse) {

}

WebXInput::~WebXInput() {
    delete _mouse;
}