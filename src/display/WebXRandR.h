#ifndef WEBX_RANDR_H
#define WEBX_RANDR_H

#include <X11/Xlib.h>
#include <X11/extensions/Xrandr.h>

class WebXRandREvent;

/**
 * \class WebXRandR
 * \brief Manages display resolution and rotation using the X11 RandR extension.
 *
 * This class provides a wrapper around the X11 RandR (Resize and Rotate) extension,
 * allowing dynamic screen resolution changes and rotation. It handles querying available
 * display modes, creating custom modes when necessary, and applying resolution changes
 * to connected outputs.
 *
 * The class encapsulates the complexity of RandR operations including mode selection,
 * CRTC (Cathode Ray Tube Controller) configuration, and error handling with automatic
 * reversion capabilities.
 */
class WebXRandR {
public:
    /**
     * \brief Constructs a WebXRandR instance for display management.
     *
     * \param x11Display Pointer to the X11 Display connection
     * \param rootWindow The root window of the X11 display
     */
    WebXRandR(Display * x11Display, Window rootWindow) :
        _x11Display(x11Display), 
        _rootWindow(rootWindow) {
    }
    
    /**
     * \brief Destructor for the WebXRandR instance.
     */
    virtual ~WebXRandR() {
    }

    /**
     * \brief Checks if the screen can be resized.
     *
     * Determines whether there is at least one connected output available
     * that supports mode changes.
     *
     * \return true if the screen can be resized, false otherwise
     */
    bool canResizeScreen() const {
        RROutput output;
        bool available = this->getConnectedOutput(&output, false);
        return available;
    }

    /**
     * \brief Resizes the screen to the specified dimensions.
     *
     * Attempts to change the screen resolution to the requested width and height.
     * If an exact matching mode exists, it will be used. Otherwise, a new custom
     * mode will be created if the hardware supports it. If the operation fails,
     * the display is reverted to its previous state.
     *
     * \param width The desired screen width in pixels
     * \param height The desired screen height in pixels
     */
    void resizeScreen(unsigned int width, unsigned int height) const;

    /**
     * \brief Validates whether an event is a valid RandR event.
     *
     * Checks if the provided event is a legitimate event (given the event screen size)
     * to determine whether toforward it to clients.
     *
     * \param event The WebXRandREvent to validate
     * \return true if the event is a valid RandR event, false otherwise
     */
    bool isValidRandREvent(const WebXRandREvent & event) const;

private:
    /**
     * \brief Finds an XRR mode matching the specified dimensions.
     *
     * Searches the available display modes for one that matches the given
     * width and height dimensions.
     *
     * \param width The desired width in pixels
     * \param height The desired height in pixels
     * \return Pointer to the matching XRRModeInfo, or nullptr if no match found
     */
    XRRModeInfo * getMatchingModeInfo(int width, int height) const;

    /**
     * \brief Gets mode information for a specific RandR mode ID.
     *
     * Retrieves the XRRModeInfo structure associated with the given mode ID.
     *
     * \param mode The RandR mode identifier
     * \return Pointer to the XRRModeInfo for the given mode ID
     */
    XRRModeInfo * getMatchingModeInfo(RRMode mode) const;

    /**
     * \brief Finds a connected output/monitor on the display.
     *
     * Searches for an output that is currently connected to the display.
     *
     * \param connectedOutput Output parameter that will receive the RROutput handle
     * \param useFallback If true, will return a fallback output if no connected one is found
     * \return true if a connected output was found, false otherwise
     */
    bool getConnectedOutput(RROutput * connectedOutput, bool useFallback) const;

    /**
     * \brief Creates a new custom display mode with the specified dimensions.
     *
     * Attempts to create a new XRR mode for the given width and height.
     * This is used when a standard mode matching the requested resolution
     * is not available.
     *
     * \param width The width in pixels for the new mode
     * \param height The height in pixels for the new mode
     * \return Pointer to the newly created XRRModeInfo, or nullptr if creation failed
     */
    XRRModeInfo * createMode(int width, int height) const;

    /**
     * \brief Sets an output to use the specified display mode.
     *
     * Configures the CRTC (Cathode Ray Tube Controller) for the given output
     * to use the specified mode at the requested resolution. If setting the mode
     * fails then the CRTC is reverted to the previous mode.
     *
     * \param output The RROutput to configure
     * \param modeInfo Pointer to the XRRModeInfo to apply
     * \param currentWidth The current screen width in pixels
     * \param currentHeight The current screen height in pixels
     * \return true if the mode was successfully applied, false otherwise
     */
    bool setOutputToMode(RROutput output, XRRModeInfo * modeInfo, int currentWidth, int currentHeight) const;

    /**
     * \brief Reverts the display configuration to a previous state.
     *
     * Restores the CRTC configuration to its state before an attempted
     * mode change. Used when a resize operation fails to maintain display stability.
     *
     * \param s The status code from the failed operation
     * \param crtc The CRTC to revert
     * \param crtcInfo Pointer to the saved CRTC configuration information
     * \param screenResources Pointer to the screen resources information
     * \param width The previous width to restore
     * \param height The previous height to restore
     */
    void revert(Status s, RRCrtc crtc, XRRCrtcInfo * crtcInfo, XRRScreenResources * screenResources, int width, int height) const;

    /**
     * \brief Deletes a custom display mode.
     *
     * Removes a previously created custom XRR mode from the display server.
     *
     * \param modeInfo Pointer to the XRRModeInfo of the mode to delete
     */
    void deleteMode(XRRModeInfo * modeInfo) const;

    /**
     * \brief Cleans up unused custom display modes.
     *
     * Removes custom modes that are no longer in use.
     *
     * \param currentModeInfo Pointer to the currently active mode to preserve
     */
    void cleanupModes(XRRModeInfo * currentModeInfo) const;

private:
    Display * _x11Display;
    Window _rootWindow;
};

#endif /* WEBX_RANDR_H */