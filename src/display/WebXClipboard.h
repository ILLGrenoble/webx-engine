#ifndef WEBX_CLIPBOARD_H
#define WEBX_CLIPBOARD_H

#include <X11/Xlib.h>
#include <X11/Xatom.h>
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
        _targets(XInternAtom(x11Display, "TARGETS", False)),
        _webXClipboardProperty(XInternAtom(x11Display, "WEBX_CLIPBOARD", False)),
        _clipboardWindow(XCreateSimpleWindow(x11Display, rootWindow, -10, -10, 1, 1, 0, 0, 0)) {
    }
    
    virtual ~WebXClipboard() {
        XDestroyWindow(this->_x11Display, this->_clipboardWindow);
        XFlush(this->_x11Display);
    }

    void updateClipboard() {
        std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> durationMs = now - this->_lastClipboardReadTime;
        if (durationMs.count() > CLIPBOARD_REFRESH_RATE_MS) {
            // Request clipboard content from X11.... and wait for the response
            Window clipboardOwner = XGetSelectionOwner(this->_x11Display, this->_clipboard);
            if (clipboardOwner != None && clipboardOwner != this->_clipboardWindow) {
                XConvertSelection(this->_x11Display, this->_clipboard, this->_utf8, this->_webXClipboardProperty, this->_clipboardWindow, CurrentTime);
            }

            this->_lastClipboardReadTime = now;
        }
    }

    void onClipboardDataNotify(const XSelectionEvent * selectionEvent) {
        if (selectionEvent->property == None) {
            return;
        }
        
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

   void onClipboardContentRequest(const XSelectionRequestEvent * selectionRequestEvent) const {

        XSelectionEvent ssev;
        ssev.type = SelectionNotify;
        ssev.requestor = selectionRequestEvent->requestor;
        ssev.selection = selectionRequestEvent->selection;
        ssev.target = selectionRequestEvent->target;
        ssev.time = selectionRequestEvent->time;

        if (selectionRequestEvent->target == this->_targets) {
            Atom supportedTargets[] = { this->_utf8, XA_STRING, this->_targets };
            XChangeProperty(this->_x11Display, selectionRequestEvent->requestor, selectionRequestEvent->property, XA_ATOM, 32, PropModeReplace, reinterpret_cast<unsigned char *>(supportedTargets), 2);
            ssev.property = selectionRequestEvent->property;
            
        } else if (selectionRequestEvent->target == this->_utf8 || selectionRequestEvent->target == XA_STRING) {
            XChangeProperty(this->_x11Display, selectionRequestEvent->requestor, selectionRequestEvent->property, this->_utf8, 8, PropModeReplace, (unsigned char *)this->_clipboardContent.c_str(), this->_clipboardContent.size());
            ssev.property = selectionRequestEvent->property;
        
        } else {
            ssev.property = None;
        }

        XSendEvent(this->_x11Display, selectionRequestEvent->requestor, True, NoEventMask, (XEvent *)&ssev);
   }

   void setClipboardContent(const std::string & content) {
        this->_clipboardContent = content;
        XSetSelectionOwner(this->_x11Display, this->_clipboard, this->_clipboardWindow, CurrentTime);
    }

private:
    const static int CLIPBOARD_REFRESH_RATE_MS = 500;

    Display * _x11Display;
    const Atom _clipboard;
    const Atom _utf8;
    const Atom _targets;
    const Atom _webXClipboardProperty;
    Window _clipboardWindow;

    std::function<void(const std::string & content)> _onClipboardContentChange;

    std::chrono::high_resolution_clock::time_point _lastClipboardReadTime;
    std::string _clipboardContent;
};

#endif /* WEBX_CLIPBOARD_H */