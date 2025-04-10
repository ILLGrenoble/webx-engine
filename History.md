1.1.1 10/04/2025
================
 * Send cursor notification message to client that sends mouse instruction: updates the cursor but not the position of the client mouse. Fixes bug that client mouse cursor never updated.

1.1.0 09/04/2025
================
 * Modify handling of mouse messages: when receiving a mouse instruction, immediately send messages to all other clients to notify them. Regularly check if mouse position has been set outside of webx.
 * Modify settings defaults depending on event filtering configuration: if filtering enabled, default to false the checksum verification
 * Add settings options to enable/disable the filter of damage events that occur immediately after a configuration notify.
 * Add settings option to disable the checksum generation of images: reduces CPU but increase bandwidth.
 * Filter DamangeNotify events that occur immediately after a ConfigureNotify event (created excessive load on the window checksum calculations and do not indicate that any damage has occurred to the window)
 * Add messages and instructions ot include clipboard into the controller.
 * Handle setting the clipboard content in the X11 server and handling requests to obtain the clipboard content. 
 * Add Clipboard class to obtain current clipboard value of X11 windows.

1.0.0 01/04/2025
================
 * Add release builds for ARM architecture
 * Full code documentation
 * Add release build for Ubuntu 20.04 
 * Fix bug: avoid removing damage listener when retreiving window images to avoid graphical discrepencies
 * Fix bug: handle numerical keyboard correctly
 * Add maximum quality settings from client and send quality data to client
 * Modify quality group of each client depending on ratio of image transfer rate from engine to network datarate calculated for client
 * Calculate mean bitrate of each individual client through ACK instructions as response to image transfer messages
 * Calculate mean and standard-deviation Round-Trip Time latency between engine and each client
 * Handle ping-pong messages from engine to each client: disconnect clients if no response after 10s (send disconnect message to relay)
 * Remove window image updates from WebXController and delegate to each individual group (each with different refresh times)
 * Quality management for each group using ClientWindow (representing update times and pending damage for each quality level)
 * Add client index mask to all messages: messages can be sent to individual, groups or all clients depending on bitmask value
 * Ignore instructions that come from unknown clients
 * client added/removed through connection/disconnection request: assigned unique Id and Index bitmask
 * Handle multi-client connections: group clients by quality
 * Add settings to manage window quality options
 * Refactoring of directory structure; separation between models and utils classes
 * Adjust quality of window quickly when it is brought to top, reduce slowly when covered
 * Limit image transfer bitrate for each individual window (max at 12 Mbps), modify according to quality
 * Improve window image quality if mouse is over a visible portion of a window
 * Performance improvement: only update window layout once per frame
 * Separate image quality for alpha image
 * Calculate overlap coverage of windows: modify quality if window is more covered
 * Independent quality level applied to each window
 * Removal on managed windows in WebXDisplay (obsoleted code)
 * Loose coupling between transport and controller layers using a gateway. Remove all singletons.
 * Generation of WebXWindowProperties on request (rather than maintaining permanent separate list)
 * Metrics test script
 * Performance improvement on alpha image creation: use unrolled loop
 * Performance improvement on transparency check: exit on first transparent pixel

0.1.4 10/02/2025
================
 * Build releases versions also for Debian 12 and Ubuntu 24
 * Remove WebXKeyboardConnection and unused libraries
 * Update spdlog to 1.15.1
 * Cleanup tests

0.1.3 28/01/2025
================
 * Fix subscriber filtering for newer versions of zmq
 * Build in release mode when creating the debian pacakge
 * Remove test loop for creation of alpha map (forgotten remnant of perf testing)
 * Update README
 * Add BSD license
 * Add multi-user devcontainer for full webx stack dev
 * Add devcontainer to develop webx-engine inside the webx-dev-env container
 * Add preprocessor flags to compile for newer versions of ZMQ (fix deprecated warnings too)

0.1.2 02/01/2025
================
 * Set the package version in the ci/cd automatically to the tag value

0.1.1 18/12/2024
================
 * Add github action to build and upload debian packages to release files

0.1.0 17/02/2023
================
 * Initial release

