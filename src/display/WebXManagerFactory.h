#ifndef WEBX_MANAGER_FACTORY_H
#define WEBX_MANAGER_FACTORY_H

#include <display/x11/WebXx11Manager.h>

static WebXManager * buildWebXManager(const WebXSettings & settings, const std::string & keyboardLayout = "", bool rootWindowMode = false) {
    return new WebXx11Manager(settings, keyboardLayout, rootWindowMode);
}


#endif /* WEBX_DISPLAY_FACTORY_H */