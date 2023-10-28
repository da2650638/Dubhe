#include "event.h"

#include "core/logger.h"
#include "core/dmemory.h"
#include "containers/darray.h"

typedef struct registered_event {
    void* listener;
    PFN_on_event callback;
}registered_event;

typedef struct event_code_entry {
    registered_event* events; // Dynamic array of the events
} event_code_entry;

#define MAX_EVENT_CODES 16384

// Event state structure
typedef struct event_system_state {
    event_code_entry registered_events[MAX_EVENT_CODES];
} event_system_state;

static b8 is_event_system_initialized = FALSE;
static event_system_state event_state;

b8 event_initialize()
{
    if(is_event_system_initialized == TRUE)
    {
        DERROR("Event system has been initialized.");
        return FALSE;
    }
    is_event_system_initialized = TRUE;
    dzero_memory(&event_state, sizeof(event_system_state));

    return TRUE;
}

void event_shutdown()
{
    for(u16 i = 0; i < MAX_EVENT_CODES; i++)
    {
        darray_destroy((void*)(event_state.registered_events[i].events));
        event_state.registered_events[i].events = 0;
    }
}

b8 event_register(u16 code, void* listener, PFN_on_event callback)
{
    if(is_event_system_initialized == FALSE)
    {
        return FALSE;
    }

    if(event_state.registered_events[code].events == 0)
    {
        event_state.registered_events[code].events = darray_create(registered_event);
    }

    u64 registered_count = darray_length(event_state.registered_events[code].events);
    for(u64 i = 0; i < registered_count; i++)
    {
        if(event_state.registered_events[code].events[i].listener == listener)
        {
            DWARN("The listener is already registered.");
            return FALSE;
        }
    }
    registered_event new_event;
    new_event.callback = callback;
    new_event.listener = listener;
    darray_push(event_state.registered_events[code].events, new_event);

    return TRUE;
}

b8 event_unregister(u16 code, void* listener, PFN_on_event callback)
{
    if(is_event_system_initialized == FALSE)
    {
        return FALSE;
    }

    if(event_state.registered_events[code].events == 0)
    {
        // TODO: warn
        return FALSE;
    }

    u64 registered_count = darray_length(event_state.registered_events[code].events);
    u64 index;
    for(index = 0; index < registered_count; index++)
    {
        if(event_state.registered_events[code].events[index].listener == listener && 
           event_state.registered_events[code].events[index].callback == callback)
        {
            registered_event event;
            darray_pop_at(event_state.registered_events[code].events, index, &event);
            return TRUE;
        }
    }

    // Not Found
    return FALSE;
}

b8 event_fire(u16 code, void* sender, event_context data)
{
    if(is_event_system_initialized == FALSE)
    {
        return FALSE;
    }

    if(event_state.registered_events[code].events == 0)
    {
        // TODO: warn
        return FALSE;
    }
    u64 registered_count = darray_length(event_state.registered_events[code].events);
    for(u64 i = 0; i < registered_count; i++)
    {
        registered_event e = event_state.registered_events->events[i];
        if(e.callback(code, sender, e.listener, data))
        {
            // NOTE: 这个返回基本上不影响事件处理流程
            return TRUE;
        }
    }

    // NOTE: 这个返回基本上不影响事件处理流程
    return FALSE;
}