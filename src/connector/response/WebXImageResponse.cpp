#include "WebXImageResponse.h"
#include <image/WebXImage.h>
#include <base64/base64.h>

void WebXImageResponse::toJson(nlohmann::json & j) const {
    j = nlohmann::json{
            {"windowId", this->_windowId},
            {"depth", this->_image->getDepth()},
            {"data", "data:image/" + this->_image->getFileExtension() + ";base64," + base64_encode(this->_image->getRawData(), this->_image->getRawDataSize())}
        };
}
