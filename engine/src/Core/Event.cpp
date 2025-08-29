#include "Event.hpp"

#include "Memory/List.hpp"

List<PFN_OnEvent> eventTypeToFunctions[EVENT_TYPE_EVENT_LAST];


void Event_Initialize() {
    for(u32 i = 0; i < EVENT_TYPE_EVENT_LAST; i++) {
        List_Create(&eventTypeToFunctions[i]);
    }
}   

void Event_Shutdown() {
    RPR_DEBUG("Event_Shutdown");

    for(u32 i = 0; i < EVENT_TYPE_EVENT_LAST; i++) {
        List_Destroy(&eventTypeToFunctions[i]);
    }
}

RPR_API void Event_AddListener(Event_Type eventType, PFN_OnEvent function) {
    for(u32 i = 0; i < eventTypeToFunctions[eventType].size; i++) {
        if(eventTypeToFunctions[eventType].data[i] == function) {
            RPR_WARN("Event_AddListener: function already subscribed, returning...");
            return;
        }
    }
    List_PushBack(&eventTypeToFunctions[eventType], function);
}

RPR_API void Event_RemoveListener(Event_Type eventType, PFN_OnEvent function) {
    for(u32 i = 0; i < eventTypeToFunctions[eventType].size; i++) {
        if(eventTypeToFunctions[eventType].data[i] == function) {
            List_Remove(&eventTypeToFunctions[eventType], function);
            return;
        }
    }
    RPR_WARN("Event_RemoveListener: function to remove not found");
}

RPR_API void Event_Fire(Event_Type eventType, Event event) {
    for(u32 i = 0; i < eventTypeToFunctions[eventType].size; i++) {
        PFN_OnEvent current = eventTypeToFunctions[eventType].data[i];
        current(event);
    }
}