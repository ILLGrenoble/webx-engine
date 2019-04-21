#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <unistd.h>
#include <X11/extensions/XTest.h>
#include <chrono>

/**
 * Move the pointer to a given position
 */
void moveTo(Display *display, double x, double y)
{
    int screen = XDefaultScreen(display);
    Window rootWindow = XRootWindow(display, screen);
    XWarpPointer(display, 0L, rootWindow, 0, 0, 0, 0, x, y);
    XFlush(display);
}

void click(Display *display, const unsigned int button)
{
    // press button
    XTestFakeButtonEvent(display, button, True, 0);
    XFlush(display);
    // release button
    XTestFakeButtonEvent(display, button, False, 0);
    XFlush(display);
}

void scrollDown(Display *display)
{
    XTestFakeButtonEvent(display, 5, True, 0);
    XFlush(display);
    XTestFakeButtonEvent(display, 5, False, 0);
    XFlush(display);
}

int main()
{
    auto display = XOpenDisplay(NULL);
    if (display == NULL)
    {
        printf("Could not attach to the XServer. Please verify the DISPLAY environment variable.\n");
        exit(EXIT_FAILURE);
    }

    printf("Sleeping for 3 seconds....\n");

    printf("Moving cursor to 500x400....\n");
    moveTo(display, 800.0, 500.0);
    printf("Sleeping for 3 seconds...\n");
    usleep(3000000);
    printf("Left clicking....\n");
    click(display, 2);
    printf("Sleeping for 3 seconds...\n");
    usleep(3000000);
    click(display, 3);
    printf("Sleeping for 3 seconds...\n");
    printf("Scroll down...\n");
    usleep(3000000);
    scrollDown(display);
}