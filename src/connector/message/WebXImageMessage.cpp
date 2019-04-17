#include "WebXImageMessage.h"
#include <image/WebXImage.h>
#include <base64/base64.h>

void WebXImageMessage::toJson(nlohmann::json & j) const {
    j = nlohmann::json{
        {"type", "image"},
        {"windowId", this->_windowId},
        {"depth", this->_image->getDepth()},
        {"data", "data:image/" + this->_image->getFileExtension() + ";base64," + base64_encode(this->_image->getRawData(), this->_image->getRawDataSize())}
    };
}
