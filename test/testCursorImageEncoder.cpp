#include <X11/Xlib.h>
#include <X11/extensions/Xfixes.h>
#include <chrono>
#include <input/WebXMouse.h>
#include <input/cursor/WebXMouseCursorFactory.h>
#include <image/WebXImage.h>

int main() {
    Display * display = XOpenDisplay(NULL);
    // Convert raw image data to WebXImage
    int screen = XDefaultScreen(display);
    Window rootWindow = XRootWindow(display, screen);
    WebXMouse mouse(display, rootWindow);
    WebXMouseCursorFactory cursorFactory(display);

    XFixesCursorImage * cursor = XFixesGetCursorImage(display);
    int nIter = 60;
    double cummulativeTimeUs = 0;
    size_t fileSize = 0;
    for (int i = 0; i < nIter; i++) {
        std::shared_ptr<WebXImage> image = cursorFactory.convertCursorImage(cursor);
        cummulativeTimeUs += image->getEncodingTimeUs();
        if (i == 0) {
            fileSize = image->getRawDataSize();
            std::string outputFilename = "test/output/cursor-1.png";
            if (image->save(outputFilename)) {
                printf("Output image saved to %s\n", outputFilename.c_str());
            } else {
                printf("Failed to save image\n");
            }
        }
    }

    printf("Text completed: %d iterations in %fms\n%fms / iteration for %lu bytes\n", nIter, cummulativeTimeUs / 1000,(cummulativeTimeUs / nIter) / 1000, fileSize);
}
