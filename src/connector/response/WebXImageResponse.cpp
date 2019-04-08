#include "WebXImageResponse.h"
#include <image/WebXImage.h>
#include <base64/base64.h>

void WebXImageResponse::toJson(nlohmann::json & j) const {
    j = nlohmann::json{
            {"data", base64_encode(this->_image->getRawData(), this->_image->getRawDataSize())}
        };

}
