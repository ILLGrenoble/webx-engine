#ifndef WEBX_RANDR_H
#define WEBX_RANDR_H

#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

class WebXRandREvent;

class WebXRandR {
public:
    WebXRandR(Display * x11Display, Window rootWindow) :
        _x11Display(x11Display), 
        _rootWindow(rootWindow) {
    }
    
    virtual ~WebXRandR() {
    }

    bool canResizeScreen() const {
        RROutput output;
        bool available = this->getConnectedOutput(&output, false);
        return available;
    }

    void resizeScreen(unsigned int width, unsigned int height) const;

    bool isValidRandREvent(const WebXRandREvent & event) const;

private:
    XRRModeInfo * getMatchingModeInfo(int width, int height) const;
    XRRModeInfo * getMatchingModeInfo(RRMode mode) const;
    bool getConnectedOutput(RROutput * connectedOutput, bool useFallback) const;
    XRRModeInfo * createMode(int width, int height) const;
    bool setOutputToMode(RROutput output, XRRModeInfo * modeInfo, int currentWidth, int currentHeight) const;
    void revert(Status s, RRCrtc crtc, XRRCrtcInfo * crtcInfo, XRRScreenResources * screenResources, int width, int height) const;
    void deleteMode(XRRModeInfo * modeInfo) const;
    void cleanupModes(XRRModeInfo * currentModeInfo) const;

private:
    Display * _x11Display;
    Window _rootWindow;
};

#endif /* WEBX_RANDR_H */