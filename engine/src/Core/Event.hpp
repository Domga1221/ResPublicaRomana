#pragma once 

#include "defines.hpp"

// TODO: not sure if this is the right approach for events right now
// 32 bytes 
struct Event {
    union {
        u16 WindowResize[2];
        u16 KeyPressed;
        u16 MouseButtonPressed;
    };
};

enum Event_Type {
    EVENT_TYPE_WINDOW_RESIZE = 0,
    EVENT_TYPE_KEY_PRESSED,
    EVENT_TYPE_MOUSE_BUTTON_PRESSED,

    EVENT_TYPE_APPLICATION_QUIT,

    EVENT_TYPE_EVENT_LAST,
};

void Event_Initialize();
void Event_Shutdown();

// TODO: Maybe use bool to signal that event was handled and doesn't need to be passed to other listeners
typedef void (*PFN_OnEvent)(Event event);

RPR_API void Event_AddListener(Event_Type eventType, PFN_OnEvent);
RPR_API void Event_RemoveListener(Event_Type eventType, PFN_OnEvent);
RPR_API void Event_Fire(Event_Type eventType, Event event);