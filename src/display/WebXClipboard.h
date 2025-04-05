#ifndef WEBX_CLIPBOARD_H
#define WEBX_CLIPBOARD_H

#include <X11/Xlib.h>
#include <chrono>
#include <functional>
#include <string>

class WebXClipboard {
public:
    WebXClipboard(Display * x11Display, Window rootWindow, std::function<void(const std::string & content)> onClipboardContentChange) :
        _x11Display(x11Display),
        _onClipboardContentChange(onClipboardContentChange),
        _clipboard(XInternAtom(x11Display, "CLIPBOARD", False)),
        _utf8(XInternAtom(x11Display, "UTF8_STRING", False)),
        _webXClipboardProperty(XInternAtom(x11Display, "WEBX_CLIPBOARD", False)),
        _clipboardWindow(XCreateSimpleWindow(x11Display, rootWindow, -10, -10, 1, 1, 0, 0, 0)) {
    }
    virtual ~WebXClipboard() {
    }

    void updateClipboard() {
        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> durationMs = now - this->_lastClipboardReadTime;
        if (durationMs.count() > CLIPBOARD_REFRESH_RATE_MS) {
            // Request clipboard content from X11.... and wait for the response
            Window clipboardOwner = XGetSelectionOwner(this->_x11Display, this->_clipboard);
            if (clipboardOwner != None) {
                XConvertSelection(this->_x11Display, this->_clipboard, this->_utf8, this->_webXClipboardProperty, this->_clipboardWindow, CurrentTime);
            }

            this->_lastClipboardReadTime = now;
        }
    }

    void onClipboardDataReceived() {
        Atom type;
        int format;
        unsigned long size, dummy;
        unsigned char * content = NULL;
    
        /* Dummy call to get type and size. */
        XGetWindowProperty(this->_x11Display, this->_clipboardWindow, this->_webXClipboardProperty, 0, 0, False, AnyPropertyType, &type, &format, &dummy, &size, &content);
        XFree(content);
    
        Atom incr = XInternAtom(this->_x11Display, "INCR", False);
        if (type == incr) {
            printf("Data too large and INCR mechanism not implemented\n");
            return;
        }
    
        XGetWindowProperty(this->_x11Display, this->_clipboardWindow, this->_webXClipboardProperty, 0, size, False, AnyPropertyType, &type, &format, &dummy, &dummy, &content);
        std::string clipboardContent = content ? std::string(reinterpret_cast<char *>(content), size) : "";
        XFree(content);

        // Signal the selection owner that we have successfully read the data.
        XDeleteProperty(this->_x11Display, this->_clipboardWindow, this->_webXClipboardProperty);

        if (clipboardContent != this->_clipboardContent) {
            this->_clipboardContent = clipboardContent;
            this->_onClipboardContentChange(this->_clipboardContent);
        }
   }

private:
    const static int CLIPBOARD_REFRESH_RATE_MS = 500;

    Display * _x11Display;
    Atom _clipboard;
    Atom _utf8;
    Atom _webXClipboardProperty;
    Window _clipboardWindow;

    std::function<void(const std::string & content)> _onClipboardContentChange;

    std::chrono::high_resolution_clock::time_point _lastClipboardReadTime;
    std::string _clipboardContent;
};

#endif /* WEBX_CLIPBOARD_H */