#include "WebXRandR.h"
#include <utils/WebXStringUtils.h>
#include "events/WebXRandREvent.h"
#include <spdlog/spdlog.h>

void WebXRandR::resizeScreen(unsigned int width, unsigned int height) const {

    XWindowAttributes attr;
    XGetWindowAttributes(this->_x11Display, this->_rootWindow, &attr);
    int current_width  = attr.width;
    int current_height  = attr.height;

    if (width == current_width && height == current_height) {
        spdlog::debug("Requested screen size {}x{} is same as the current size", width, height);
        return;
    }

    spdlog::info("Resizing screen from {}x{} to {}x{}", current_width, current_height, width, height);

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
    if (this->setOutputToMode(output, selectedModeInfo)) {
        this->cleanupModes(selectedModeInfo);
    }
}

bool WebXRandR::isValidRandREvent(const WebXRandREvent & screenChangeEvent) const {
    XRRUpdateConfiguration((XEvent *)screenChangeEvent.event());
    XWindowAttributes attr;
    XGetWindowAttributes(this->_x11Display, this->_rootWindow, &attr);
    int current_width  = attr.width;
    int current_height  = attr.height;

    if (screenChangeEvent.getWidth() != current_width || screenChangeEvent.getHeight() != current_height) {
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

bool WebXRandR::setOutputToMode(RROutput output, XRRModeInfo * modeInfo) const {
    int current_width_mm  = DisplayWidthMM(this->_x11Display, DefaultScreen(this->_x11Display));
    int current_height_mm  = DisplayHeightMM(this->_x11Display, DefaultScreen(this->_x11Display));

    // Set screen size
    XRRSetScreenSize(this->_x11Display, this->_rootWindow, modeInfo->width, modeInfo->height, current_width_mm, current_height_mm);
    XSync(this->_x11Display, False);

    XRRScreenResources * screenResources = XRRGetScreenResources(this->_x11Display, this->_rootWindow);

    XRROutputInfo * outputInfo = XRRGetOutputInfo(this->_x11Display, screenResources, output);
    if (outputInfo == nullptr) {
        spdlog::warn("Failed to find output info for output {}", output);
        return false;
    }

    // Select node
    spdlog::debug("Setting the current output to {}x{}", modeInfo->width, modeInfo->height);
    RRCrtc crtc = outputInfo->crtc;
    XRRSetCrtcConfig(this->_x11Display, screenResources, crtc, screenResources->configTimestamp, 0, 0, modeInfo->id, RR_Rotate_0, &output, 1);

    // Resize screen and sync
    XRRSetScreenSize(this->_x11Display, this->_rootWindow, modeInfo->width, modeInfo->height, current_width_mm, current_height_mm);
    XSync(this->_x11Display, False);
    XFlush(this->_x11Display);

    // Cleanup
    XRRFreeOutputInfo(outputInfo);
    XRRFreeScreenResources(screenResources);

    return true;
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
