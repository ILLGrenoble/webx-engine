#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/extensions/Xdamage.h>
#include <X11/extensions/Xcomposite.h>
#include <events/WebXEventListener.h>
#include <spdlog/spdlog.h>
#include <X11/Xlibint.h>

static int IO_ERROR_HANDLER(Display *display) {
    fprintf(stderr, "X11 shat the bed\n");
    return 0;
}


typedef Bool (*WireToEventType) (
    Display*	/* display */,
    XEvent*	/* re */,
    xEvent*	/* event */
);

WireToEventType originalWireToEventHandler;

Window windowDamageToIgnore = 0;

static Bool wireToEventWrapper(Display *dpy, XEvent *event, xEvent *wire) {
    if (originalWireToEventHandler) {
        Bool originalReturnValue = originalWireToEventHandler(dpy, event, wire);
        if (originalReturnValue == True && windowDamageToIgnore != 0) {
            XDamageNotifyEvent * damageEvent = (XDamageNotifyEvent *) event;

            return (damageEvent->drawable != windowDamageToIgnore);
        
        } else {
            return originalReturnValue;
        }
    }

    return False;
}

struct WindowDamageProperties {
    Window window;
    WebXRectangle rectangle;
};

int testWithDamage() {

    std::vector<WindowDamageProperties> damagedWindows;
    Display * display = XOpenDisplay(NULL);

    XSetIOErrorHandler(IO_ERROR_HANDLER);

    Window rootWindow = RootWindow(display, DefaultScreen(display));
    WebXEventListener * eventListener = new WebXEventListener(display, rootWindow);

    originalWireToEventHandler = XESetWireToEvent(display, 91, wireToEventWrapper);

    bool ignoreDamage = false;

    eventListener->addEventHandler(WebXEventType::Damaged, [&damagedWindows, &ignoreDamage](const WebXEvent & event) {
        if (!ignoreDamage) {
            const WebXRectangle & rectangle = event.getRectangle();

            spdlog::info("Damage to window 0x{:x}({:d}, {:d}), {:d} x {:d}", event.getX11Window(), rectangle.x, rectangle.y, rectangle.size.width, rectangle.size.height);
            damagedWindows.push_back({event.getX11Window(), rectangle});
        } else {
            spdlog::warn("Ignore damage");
        }
    });

    Window window = 0x60061c;
    Damage damage = XDamageCreate(display, window, XDamageReportRawRectangles);

    while (1) {
        usleep(100);

        eventListener->flushQueuedEvents();

        auto copiedDamagedWindows = damagedWindows;
        damagedWindows.clear();

        for (auto it = copiedDamagedWindows.begin(); it != copiedDamagedWindows.end(); it++) {
            WindowDamageProperties & windowDamage = *it;
            const WebXRectangle & rectangle = windowDamage.rectangle;

            
            XDamageDestroy(display, damage);
            spdlog::info("Grabbing WebXWindow 0x{:x} ({:d}, {:d}), {:d} x {:d}...", window, rectangle.x, rectangle.y, rectangle.size.width, rectangle.size.height);
            XImage * image = XGetImage(display, windowDamage.window, rectangle.x, rectangle.y, rectangle.size.width, rectangle.size.height, AllPlanes, ZPixmap);
            spdlog::info("... grabbed window");
            
            XSync(display, false);

            spdlog::info("Before DamageCreate");
            
            windowDamageToIgnore = window;
            damage = XDamageCreate(display, window, XDamageReportRawRectangles);
            spdlog::info("After DamageCreate");

            XSync(display, false);
            windowDamageToIgnore = 0;

            spdlog::info("After XSync {:d}", XEventsQueued(display, QueuedAfterFlush));
            
            if (image) {
                XDestroyImage(image);

            } else {
                spdlog::error("Failed to get image for window 0x{:x}", window);
            }
        }
    }

    XDamageDestroy(display, damage);

    return 0;
}

int testWithoutDamage() {
    Display * display = XOpenDisplay(NULL);

    Window window = 0x60061c;
    WebXRectangle rectangle(3, 951, 1118, 1);

    while (1) {
        usleep(1000);
        
        spdlog::info("Grabbing WebXWindow 0x{:x} ({:d}, {:d}), {:d} x {:d}", window, rectangle.x, rectangle.y, rectangle.size.width, rectangle.size.height);
        XImage * image = XGetImage(display, window, rectangle.x, rectangle.y, rectangle.size.width, rectangle.size.height, AllPlanes, ZPixmap);

        if (image) {
            XDestroyImage(image);

        } else {
            spdlog::error("Failed to get image for window 0x{:x}", window);
        }

    }
}

int main() {
    return testWithDamage();
}
