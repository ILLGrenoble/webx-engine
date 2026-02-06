#ifndef WEBX_RANDR_H
#define WEBX_RANDR_H

#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

class WebXRandR {
public:
    WebXRandR(Display * x11Display, Window rootWindow) :
        _x11Display(x11Display), 
        _rootWindow(rootWindow),
        _previousCreatedModeInfo(nullptr) {
    }
    
    virtual ~WebXRandR() {
    }

    void resizeScreen(unsigned int width, unsigned int height);

private:
    XRRModeInfo * getMatchingModeInfo(int width, int height);
    XRRModeInfo * getMatchingModeInfo(RRMode mode);
    bool getConnectedOutput(RROutput * connectedOutput);
    XRRModeInfo * createMode(int width, int height);
    bool setOutputToMode(RROutput output, XRRModeInfo * modeInfo);
    void deleteMode(XRRModeInfo * modeInfo);

private:
    Display * _x11Display;
    Window _rootWindow;

    XRRModeInfo * _previousCreatedModeInfo; 
};

#endif /* WEBX_RANDR_H */