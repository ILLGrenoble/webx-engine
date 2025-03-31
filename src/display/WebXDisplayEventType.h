#ifndef WEBX_DISPLAY_EVENT_TYPE_H
#define WEBX_DISPLAY_EVENT_TYPE_H

/**
 * @enum WebXDisplayEventType
 * @brief Enumerates the types of display events handled by the WebX system.
 * 
 * This enumeration defines the types of events that can occur in the WebX display,
 * such as window layout changes and cursor events.
 */
typedef enum {
    WindowLayoutEvent = 0,
    CursorEvent,
} WebXDisplayEventType;

#endif /* WEBX_DISPLAY_EVENT_TYPE_H */