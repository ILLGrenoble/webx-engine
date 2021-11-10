#include <stdlib.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <unistd.h>
#include <X11/extensions/XTest.h>

/**
 * Tests assume french keyboard
 */

void typeBraceRight(Display *display) {

    // key <LVL3> {	[ ISO_Level3_Shift	]	};
    // <LVL3> =   92;
    XTestFakeKeyEvent(display, 92, True, 0);

    // key <AE12>	{ [     equal,       plus,   braceright,  dead_ogonek ]	};
    // 	<AE12> = 21;
    XTestFakeKeyEvent(display, 21, True, 0);

    // And release
    XTestFakeKeyEvent(display, 21, False, 0);

    XTestFakeKeyEvent(display, 92, False, 0);

    XFlush(display);
}

void typeEuro(Display *display) {

    // key <LVL3> {	[ ISO_Level3_Shift	]	};
    // <LVL3> =   92;
    XTestFakeKeyEvent(display, 92, True, 0);

    // key <AD03>	{ [         e,          E,     EuroSign,         cent ]	};
	// <AD03> = 26;
    XTestFakeKeyEvent(display, 26, True, 0);

    // And release
    XTestFakeKeyEvent(display, 26, False, 0);

    XTestFakeKeyEvent(display, 92, False, 0);

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

    typeBraceRight(display);
    typeEuro(display);
}