#ifndef WEBX_ERROR_HANDLER_H
#define WEBX_ERROR_HANDLER_H

#include <X11/Xlib.h>
#include <spdlog/spdlog.h>

class WebXErrorHandler {
public:
    WebXErrorHandler() {}
    virtual ~WebXErrorHandler() {}

  static const XErrorEvent & getLastError() {
    return LAST_ERROR;
  }

  static Window getLastErrorWindow() {
    return LAST_ERROR.resourceid;
  }

  static unsigned char getLastErrorCode() {
    return LAST_ERROR.error_code;
  }

  static int setLastError(Display * display, XErrorEvent * error) {
      spdlog::error("X11 error for window with id 0x{:x}, error 0x{:02x}", error->resourceid, error->error_code);
      LAST_ERROR = *error;
      return 0;
  }

private:
  static XErrorEvent LAST_ERROR;
};


#endif /* WEBX_ERROR_HANDLER_H */