#include "WebXRandR.h"
#include <utils/WebXStringUtils.h>
#include "events/WebXRandREvent.h"
#include <spdlog/spdlog.h>

void WebXRandR::resizeScreen(unsigned int requestedWidth, unsigned int requestedHeight) const {

    XWindowAttributes attr;
    XGetWindowAttributes(this->_x11Display, this->_rootWindow, &attr);
    int currentWidth  = attr.width;
    int currentHeight  = attr.height;

    int minWidth, minHeight;
    int maxWidth, maxHeight;

    XRRGetScreenSizeRange(this->_x11Display, this->_rootWindow, &minWidth, &minHeight, &maxWidth, &maxHeight);
    int width = requestedWidth > maxWidth ? maxWidth : requestedWidth < minWidth ? minWidth : requestedWidth;
    int height = requestedHeight > maxHeight ? maxHeight : requestedHeight < minHeight ? minHeight : requestedHeight;

    if (width != requestedWidth || height != requestedHeight) {
        spdlog::info("Request dimensions {}x{} are being clamped to {}x{}", requestedWidth, requestedHeight, width, height);
    }

    if (width == currentWidth && height == currentHeight) {
        spdlog::debug("Requested screen size {}x{} is same as the current size", width, height);
        return;
    }

    spdlog::info("Resizing screen from {}x{} to {}x{}", currentWidth, currentHeight, width, height);

    RRMode mode;
    RROutput output;
    if (!this->getConnectedOutput(&output, true)) {
        spdlog::error("Could not find connected output");
        return;
    }

    // Determine if mode already exists
    XRRModeInfo * createdMode = nullptr;
    XRRModeInfo * selectedModeInfo = this->getMatchingModeInfo(width, height);
    if (selectedModeInfo == nullptr) {
        // Create mode since it doesn't exist
        selectedModeInfo = this->createMode(width, height);

        // Add mode to the output
        XRRAddOutputMode(this->_x11Display, output, selectedModeInfo->id);

        // Store mode if created
        createdMode = selectedModeInfo;
    }

    // Set the mode in the output
    if (this->setOutputToMode(output, selectedModeInfo, currentWidth, currentHeight)) {
        this->cleanupModes(selectedModeInfo);

    } else if (createdMode != nullptr) {
        this->deleteMode(createdMode);
    }
}

bool WebXRandR::isValidRandREvent(const WebXRandREvent & screenChangeEvent) const {
    XRRUpdateConfiguration((XEvent *)screenChangeEvent.event());
    XWindowAttributes attr;
    XGetWindowAttributes(this->_x11Display, this->_rootWindow, &attr);
    int currentWidth  = attr.width;
    int currentHeight  = attr.height;

    if (screenChangeEvent.getWidth() != currentWidth || screenChangeEvent.getHeight() != currentHeight) {
        // stale / transitional event: ignore
        return false;
    }

    return true;
}

XRRModeInfo * WebXRandR::getMatchingModeInfo(int width, int height) const {
    XRRScreenResources * screenResources = XRRGetScreenResources(this->_x11Display, this->_rootWindow);

    XRRModeInfo * matchingModeInfo = nullptr;
    for (int i = 0; i < screenResources->nmode && matchingModeInfo == nullptr; i++) {
        XRRModeInfo * modeInfo = &screenResources->modes[i];
        if (modeInfo->width == width && modeInfo->height == height) {
            spdlog::debug("Found existing mode for screen size {}x{}: {}", width, height, modeInfo->name);
            matchingModeInfo = modeInfo;
        }
    }

    XRRFreeScreenResources(screenResources);
    return matchingModeInfo;
}

XRRModeInfo * WebXRandR::getMatchingModeInfo(RRMode mode) const {
    XRRScreenResources * screenResources = XRRGetScreenResources(this->_x11Display, this->_rootWindow);

    XRRModeInfo * matchingModeInfo = nullptr;
    for (int i = 0; i < screenResources->nmode && matchingModeInfo == nullptr; i++) {
        XRRModeInfo * modeInfo = &screenResources->modes[i];
        if (modeInfo->id == mode) {
            matchingModeInfo = modeInfo;
        }
    }

    XRRFreeScreenResources(screenResources);
    return matchingModeInfo;
}

bool WebXRandR::getConnectedOutput(RROutput * connectedOutput, bool useFallback) const {
    XRRScreenResources * screenResources = XRRGetScreenResources(this->_x11Display, this->_rootWindow);

    bool found = false;
    for (int i = 0; i < screenResources->noutput && found == false; i++) {
        
        RROutput output = screenResources->outputs[i];
        XRROutputInfo * outputInfo = XRRGetOutputInfo(this->_x11Display, screenResources, output);

        if (outputInfo->connection == RR_Connected && outputInfo->crtc != None) {
            found = true;
            *connectedOutput = output;
        }

        XRRFreeOutputInfo(outputInfo);
    }

    if (!found && useFallback && screenResources->noutput > 0) {
        spdlog::warn("Couldn't find connected output so selecting first one");
        *connectedOutput = screenResources->outputs[0];
        found = true;
    }

    XRRFreeScreenResources(screenResources);

    return found;
}

XRRModeInfo * WebXRandR::createMode(int width, int height) const {
    XRRModeInfo modeInfo;
    memset(&modeInfo, 0, sizeof(modeInfo));

    char name[100];

    modeInfo.width  = width;
    modeInfo.height = height;
    modeInfo.hTotal = width;
    modeInfo.vTotal = height;

    modeInfo.dotClock = width * height * 60;
    modeInfo.modeFlags = RR_HSyncPositive | RR_VSyncPositive;
    
    sprintf(name, "%dx%d_webx", width, height);
    modeInfo.name = (char *)name;
    modeInfo.nameLength = strlen(name);

    RRMode mode = XRRCreateMode(this->_x11Display, this->_rootWindow, &modeInfo);
    spdlog::debug("Created mode {} for screen size {}x{}", modeInfo.name, width, height);

    return this->getMatchingModeInfo(mode);
}

bool WebXRandR::setOutputToMode(RROutput output, XRRModeInfo * modeInfo, int currentWidth, int currentHeight) const {
    XRRScreenResources * screenResources = XRRGetScreenResources(this->_x11Display, this->_rootWindow);

    // Get the output
    XRROutputInfo * outputInfo = XRRGetOutputInfo(this->_x11Display, screenResources, output);
    if (outputInfo == nullptr) {
        spdlog::warn("Failed to find output info for output {}", output);
        return false;
    }

    spdlog::debug("Setting the current output to {}x{}", modeInfo->width, modeInfo->height);

    // Get the crtc info
    RRCrtc crtc = outputInfo->crtc;
    XRRCrtcInfo * crtcInfo = XRRGetCrtcInfo(this->_x11Display, screenResources, crtc);

    // Disable the crtc
    Status status = XRRSetCrtcConfig (this->_x11Display, screenResources, crtc, CurrentTime, 0, 0, None, RR_Rotate_0, NULL, 0);
    bool retval = false;
    if (status == RRSetConfigSuccess) {
        // Calculate mm width and height based on 96dpi
        int widthMM = modeInfo->width * 25.4 / 96;
        int heightMM = modeInfo->height * 25.4 / 96;

        // Set screen size
        XRRSetScreenSize(this->_x11Display, this->_rootWindow, modeInfo->width, modeInfo->height, widthMM, heightMM);

        // Set the desired mode
        status = XRRSetCrtcConfig(this->_x11Display, screenResources, crtc, CurrentTime, 0, 0, modeInfo->id, RR_Rotate_0, &output, 1);
        if (status == RRSetConfigSuccess) {
            // XRRSetScreenSize(this->_x11Display, this->_rootWindow, modeInfo->width, modeInfo->height, widthMM, heightMM);
            retval = true;

        } else {
            this->revert(status, crtc, crtcInfo, screenResources, currentWidth, currentHeight);
        }

    } else {
        this->revert(status, crtc, crtcInfo, screenResources, currentWidth, currentHeight);
    }

    // Sync
    XSync(this->_x11Display, False);

    // Cleanup
    XRRFreeCrtcInfo(crtcInfo);
    XRRFreeOutputInfo(outputInfo);
    XRRFreeScreenResources(screenResources);

    return retval;
}

void WebXRandR::revert(Status status, RRCrtc crtc, XRRCrtcInfo * crtcInfo, XRRScreenResources * screenResources, int width, int height) const {
    const char *message;
    
    switch (status) {
        case RRSetConfigSuccess: message = "Conguration succeeded"; break;
        case BadAlloc: message = "out of memory"; break;
        case RRSetConfigFailed: message = "Configuration failed"; break;
        case RRSetConfigInvalidConfigTime: message = "Invalid configuration time";  break;
        case RRSetConfigInvalidTime: message = "Invalid time"; break;
        default: message = "Unknown failure"; break;
    }

    spdlog::warn("Failed to configure crtc ({}): reverting to previous mode", message);

    int widthMM = width * 25.4 / 96;
    int heightMM = height * 25.4 / 96;
    
    // disable crtc
    XRRSetCrtcConfig (this->_x11Display, screenResources, crtc, CurrentTime, 0, 0, None, RR_Rotate_0, NULL, 0);

    // resize
    XRRSetScreenSize(this->_x11Display, this->_rootWindow, width, height, widthMM, heightMM);

    // enable previous mode
    XRRSetCrtcConfig(this->_x11Display, screenResources, crtc, CurrentTime, crtcInfo->x, crtcInfo->y, crtcInfo->mode, crtcInfo->rotation, crtcInfo->outputs, crtcInfo->noutput);
}

void WebXRandR::deleteMode(XRRModeInfo * modeInfo) const {
    spdlog::debug("Deleting the screen mode {}", modeInfo->name);
    RROutput output;
    if (this->getConnectedOutput(&output, true)) {
        XRRDeleteOutputMode(this->_x11Display, output, modeInfo->id);
    }

    XRRDestroyMode(this->_x11Display, modeInfo->id);
}

void WebXRandR::cleanupModes(XRRModeInfo * currentModeInfo) const {
    XRRScreenResources * screenResources = XRRGetScreenResources(this->_x11Display, this->_rootWindow);

    XRRModeInfo * matchingModeInfo = nullptr;
    for (int i = 0; i < screenResources->nmode && matchingModeInfo == nullptr; i++) {
        XRRModeInfo * modeInfo = &screenResources->modes[i];
        if (WebXStringUtils::hasEnding(modeInfo->name, "_webx") && modeInfo->id != currentModeInfo->id) {
            this->deleteMode(modeInfo);
        }
    }

    XRRFreeScreenResources(screenResources);

}
