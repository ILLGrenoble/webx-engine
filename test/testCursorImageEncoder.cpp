#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>
#include <chrono>
#include <input/cursor/WebXMouseCursorImageConverter.h>

int main() {
    Display * display = XOpenDisplay(NULL);
    XFixesCursorImage * cursor = XFixesGetCursorImage(display);
    WebXMouseCursorImageConverter converter;
    int nIter = 60;
    double cummulativeTimeUs = 0;
    size_t fileSize = 0;
    for (int i = 0; i < nIter; i++) {
        WebXMouseCursorImage * image = converter.convert(cursor);
        cummulativeTimeUs += image->getEncodingTimeUs();
        if (i == 0) {
            fileSize = image->getRawDataSize();
            std::string outputFilename = "test/output/cursor.png";
            if (image->save(outputFilename)) {
                printf("Output image saved to %s\n", outputFilename.c_str());
            } else {
                printf("Failed to save image\n");
            }
        }
    }

    printf("Text completed: %d iterations in %fms\n%fms / iteration for %lu bytes\n", nIter, cummulativeTimeUs / 1000,(cummulativeTimeUs / nIter) / 1000, fileSize);
}
