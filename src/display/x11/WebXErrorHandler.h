#ifndef WEBX_ERROR_HANDLER_H
#define WEBX_ERROR_HANDLER_H

#include <X11/Xlib.h>
#include <spdlog/spdlog.h>

/**
 * @class WebXErrorHandler
 * @brief Handles X11 errors and provides access to the last error details.
 * 
 * This class captures and logs X11 errors, storing the details of the last error
 * encountered. It provides methods to retrieve the last error's details, such as
 * the error code and the associated window.
 */
class WebXErrorHandler {
public:
    /**
     * @brief Constructs a WebXErrorHandler instance.
     */
    WebXErrorHandler() {}

    /**
     * @brief Destructor.
     */
    virtual ~WebXErrorHandler() {}

    /**
     * @brief Retrieves the last X11 error event.
     * @return Reference to the last XErrorEvent.
     */
    static const XErrorEvent & getLastError() {
        return LAST_ERROR;
    }

    /**
     * @brief Retrieves the window associated with the last X11 error.
     * @return X11 window ID of the last error.
     */
    static Window getLastErrorWindow() {
        return LAST_ERROR.resourceid;
    }

    /**
     * @brief Retrieves the error code of the last X11 error.
     * @return Error code of the last error.
     */
    static unsigned char getLastErrorCode() {
        return LAST_ERROR.error_code;
    }

    /**
     * @brief Sets the details of the last X11 error and logs it.
     * @param display Pointer to the X11 display.
     * @param error Pointer to the XErrorEvent.
     * @return Always returns 0.
     */
    static int setLastError(Display * display, XErrorEvent * error) {
        spdlog::error("X11 error for window with id 0x{:x}, error 0x{:02x}", error->resourceid, error->error_code);
        LAST_ERROR = *error;
        return 0;
    }

private:
    static XErrorEvent LAST_ERROR;
};

#endif /* WEBX_ERROR_HANDLER_H */