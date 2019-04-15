#include "WebXSubImagesMessage.h"
#include <image/WebXImage.h>
#include <base64/base64.h>

void WebXSubImagesMessage::toJson(nlohmann::json & j) const {
    j = nlohmann::json{
        {"type", "subimages"},
        {"windowId", this->_windowId},
        {"subImages", {}}
    };
    for (auto it = this->_images.begin(); it != this->_images.end(); it++) {
        const WebXSubImage & subImage = *it;

        j["subImages"].push_back({
            {"x", subImage.imageRectangle.x},
            {"y", subImage.imageRectangle.y},
            {"width", subImage.imageRectangle.size.width},
            {"height", subImage.imageRectangle.size.height},
            {"depth", subImage.image->getDepth()},
            {"data", base64_encode(subImage.image->getRawData(), subImage.image->getRawDataSize())}
        });
    }
}
