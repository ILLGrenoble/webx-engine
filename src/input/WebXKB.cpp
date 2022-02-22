#include "WebXKB.h"
#include <X11/XKBlib.h>
#include <X11/extensions/XKBrules.h>
#include <spdlog/spdlog.h>


std::string WebXKB::getKeyboardLayout(Display * display) const {
    WebXKBSettings settings;
    // Initialise keyboard settings from the server
    if (!this->initialiseKeyboardSettings(display, settings)) {
        spdlog::error("Couldn't get XKB properties from the server");
        return "";
    }

    return settings.layout;
}

bool WebXKB::setKeyboardLayout(Display * display, const std::string & layout) const {
    WebXKBSettings settings;
    // Initialise keyboard settings from the server
    if (!this->initialiseKeyboardSettings(display, settings)) {
        spdlog::error("Couldn't get XKB properties from the server");
        return false;
    }

    // set the desired layout
    settings.layout = layout;
    settings.rules = "evdev";
    settings.model = "pc105";

    // Apply the rules with the new layout
    if (!this->applyRules(settings)) {
        return false;
    }

    // Send the settings to X11
    if (this->sendKeyboardSettings(display, settings)) {
        spdlog::debug("Updated the XKB setting to use '{:s}' layout", layout);
        return true;

    } else {
        spdlog::error("Failed to update the XKB settings to use '{:s}' layout", layout);
        return false;
    }
}

bool WebXKB::initialiseKeyboardSettings(Display * display, WebXKBSettings & settings) const {
    XkbRF_VarDefsRec vd;
    char *rules = NULL;

    if (!XkbRF_GetNamesProp(display, &rules, &vd) || !rules) {
        return false;
    
    } else {
        if (rules) {
            settings.rules = rules;
        }
        if (vd.model) {
            settings.model = vd.model;
        }
        if (vd.layout) {
            settings.layout = vd.layout;
        }
        if (vd.variant) {
            settings.variant = vd.variant;
        }

        return true;
    }
}

bool WebXKB::applyRules(WebXKBSettings & settings) const {

    // Load the rules
    char * locale = NULL;
    locale = setlocale(LC_ALL, locale);

    std::string rulePath = WEBX_XBD_RULES_PATH + settings.rules;

    spdlog::debug("Loading XKB rules file {:s}...", rulePath);
    XkbRF_RulesPtr rules = XkbRF_Load((char *)rulePath.c_str(), locale, True, True);
    if (rules) {
        spdlog::debug("... loaded XKB rules file {:s}", rulePath);

        // Get details from rules
        XkbComponentNamesRec rnames;

        XkbRF_VarDefsRec rdefs;
        rdefs.model = settings.model.empty() ? NULL : (char *)settings.model.c_str();
        rdefs.layout = settings.layout.empty() ? NULL : (char *)settings.layout.c_str();
        rdefs.variant = settings.variant.empty() ? NULL : (char *)settings.variant.c_str();

        XkbRF_GetComponents(rules, &rdefs, &rnames);
        if (rnames.keycodes) {
            settings.keycodes = rnames.keycodes;
        }
        if (rnames.symbols) {
            settings.symbols = rnames.symbols;
        }
        if (rnames.types) {
            settings.types = rnames.types;
        }
        if (rnames.compat) {
            settings.compat = rnames.compat;
        }
        if (rnames.geometry) {
            settings.geometry = rnames.geometry;
        }
        if (rnames.keymap) {
            settings.keymap = rnames.keymap;
        }
        return true;

    } else {
        spdlog::error("... failed to load XKB rules file {:s}", rulePath);
        return false;
    }

}

bool WebXKB::sendKeyboardSettings(Display * display, WebXKBSettings & settings) const {

    XkbComponentNamesRec cmdNames = {
        .keymap = settings.keymap.empty() ? NULL : (char *)settings.keymap.c_str(),
        .keycodes = settings.keycodes.empty() ? NULL : (char *)settings.keycodes.c_str(),
        .types = settings.types.empty() ? NULL : (char *)settings.types.c_str(),
        .compat = settings.compat.empty() ? NULL : (char *)settings.compat.c_str(),
        .symbols = settings.symbols.empty() ? NULL : (char *)settings.symbols.c_str(),
        .geometry = settings.geometry.empty() ? NULL : (char *)settings.geometry.c_str()
    };

    int deviceSpec = XkbUseCoreKbd;

    XkbDescPtr xkb = XkbGetKeyboardByName(display, deviceSpec, &cmdNames,
                                   XkbGBN_AllComponentsMask,
                                   XkbGBN_AllComponentsMask &
                                   (~XkbGBN_GeometryMask), True);
    if (xkb) {
        XkbRF_VarDefsRec rdefs;
        const char * options = "";
        rdefs.model = settings.model.empty() ? NULL : (char *)settings.model.c_str();
        rdefs.layout = settings.layout.empty() ? NULL : (char *)settings.layout.c_str();
        rdefs.variant = settings.variant.empty() ? NULL : (char *)settings.variant.c_str();
        rdefs.options = (char *)options;
        
        // update the XKB root property
        if (!settings.rules.empty() && (rdefs.model || rdefs.layout)) {
            if (XkbRF_SetNamesProp(display, (char *)settings.rules.c_str(), &rdefs)) {
                return true;
            }
        }
    }

    return false;
}
