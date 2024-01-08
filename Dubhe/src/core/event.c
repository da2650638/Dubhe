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

static event_system_state* state_ptr;

void event_system_initialize(u64* memory_requirement, void* state)
{
    *memory_requirement = sizeof(event_system_state);
    if (state == 0) 
    {
        return;
    }
    dzero_memory(state, sizeof(state));
    state_ptr = state;
}

void event_system_shutdown()
{
    if(state_ptr)
    {
        for(u16 i = 0; i < MAX_EVENT_CODES; i++)
        {
            if(state_ptr->registered_events[i].events != 0)
            {
                darray_destroy((void*)(state_ptr->registered_events[i].events));
                state_ptr->registered_events[i].events = 0;
            }
        }
    }
    state_ptr = 0;
}

b8 event_register(u16 code, void* listener, PFN_on_event callback)
{
    if(!state_ptr)
    {
        return false;
    }

    if(state_ptr->registered_events[code].events == 0)
    {
        state_ptr->registered_events[code].events = darray_create(registered_event);
    }

    u64 registered_count = darray_length(state_ptr->registered_events[code].events);
    for(u64 i = 0; i < registered_count; i++)
    {
        if(state_ptr->registered_events[code].events[i].listener == listener)
        {
            DWARN("The listener is already registered.");
            return false;
        }
    }
    registered_event new_event;
    new_event.callback = callback;
    new_event.listener = listener;
    darray_push(state_ptr->registered_events[code].events, new_event);

    return true;
}

b8 event_unregister(u16 code, void* listener, PFN_on_event callback)
{
    if(!state_ptr)
    {
        return false;
    }

    if(state_ptr->registered_events[code].events == 0)
    {
        // TODO: warn
        return false;
    }

    u64 registered_count = darray_length(state_ptr->registered_events[code].events);
    u64 index;
    for(index = 0; index < registered_count; index++)
    {
        registered_event e = state_ptr->registered_events[code].events[index];
        if(e.listener == listener && e.callback == callback)
        {
            registered_event event;
            darray_pop_at(state_ptr->registered_events[code].events, index, &event);
            return true;
        }
    }

    // Not Found
    return false;
}

b8 event_fire(u16 code, void* sender, event_context data)
{
    if(!state_ptr)
    {
        return false;
    }

    if(state_ptr->registered_events[code].events == 0)
    {
        // TODO: warn
        return false;
    }
    u64 registered_count = darray_length(state_ptr->registered_events[code].events);
    for(u64 i = 0; i < registered_count; i++)
    {
        registered_event e = state_ptr->registered_events[code].events[i];
        if(e.callback(code, sender, e.listener, data))
        {
            // NOTE: 这个返回基本上不影响事件处理流程
            return true;
        }
    }

    // NOTE: 这个返回基本上不影响事件处理流程
    return false;
}