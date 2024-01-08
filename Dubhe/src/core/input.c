#include "input.h"

#include "core/asserts.h"
#include "core/dmemory.h"
#include "core/logger.h"
#include "core/event.h"

typedef struct keyboard_state{
    b8 keys[256];
} keyboard_state;

typedef struct mousebutton_state{
    i16 x;
    i16 y;
    b8 buttons[MOUSEBUTTONS_COUNT];
} mousebutton_state;

typedef struct input_state{
    keyboard_state keyboard_current;
    keyboard_state keyboard_previous;
    mousebutton_state mousebutton_current;
    mousebutton_state mousebutton_previous;
} input_state;

static input_state* state_ptr;

void input_system_initialize(u64* memory_requirement, void* state)
{
    *memory_requirement = sizeof(input_state);
    if(!state)
    {
        return;
    }
    dzero_memory(state, sizeof(state));
    state_ptr = state;
    DINFO("Input subsystem initialized.");
}

void input_system_shutdown(void* state)
{
    // TODO: Add shutdown routines when needed.
    state_ptr = 0;
}

void input_update(f64 delta_time)
{
    if(!state_ptr)
    {
        return;
    }

    // Copy current states to previous states.
    dcopy_memory(&state_ptr->keyboard_previous, &state_ptr->keyboard_current, sizeof(state_ptr->keyboard_current));
    dcopy_memory(&state_ptr->mousebutton_previous, &state_ptr->mousebutton_current, sizeof(state_ptr->mousebutton_current));
}

void input_process_key(key_code key, b8 pressed)
{
    if(state_ptr->keyboard_current.keys[key] != pressed)
    {
        state_ptr->keyboard_current.keys[key] = pressed;

        event_context data;
        data.data.u16[0] = key;
        event_fire(pressed ? EVENT_CODE_KEY_PRESSED : EVENT_CODE_KEY_RELEASED,
        0,
        data);
    }
}

void input_process_button(mouse_button button, b8 pressed)
{
    if(state_ptr->mousebutton_current.buttons[button] != pressed)
    {
        state_ptr->mousebutton_current.buttons[button] = pressed;

        event_context data;
        data.data.u16[0] = button;
        event_fire(pressed ? EVENT_CODE_BUTTON_PRESSED : EVENT_CODE_BUTTON_RELEASED,
        0,
        data);
    }
}

void input_process_mouse_move(i16 x, i16 y)
{
    if(state_ptr->mousebutton_current.x != x || state_ptr->mousebutton_current.y != y)
    {
        state_ptr->mousebutton_current.x = x;
        state_ptr->mousebutton_current.y = y;

        event_context data;
        data.data.u16[0] = x;
        data.data.u16[1] = y;
        event_fire(EVENT_CODE_MOUSE_MOVE, 0, data);
    }
}

void input_process_mouse_wheel(i8 z_delta)
{
    event_context data;
    data.data.u8[0] = z_delta;
    event_fire(EVENT_CODE_MOUSE_WHEEL, 0, data);
}

b8 input_is_key_down(key_code key)
{
    if(!state_ptr)
    {
        return false;
    }
    return state_ptr->keyboard_current.keys[key] == true;
}

b8 input_was_key_down(key_code key)
{
    if(!state_ptr)
    {
        return false;
    }
    return state_ptr->keyboard_previous.keys[key] == true;
}

b8 input_is_key_up(key_code key)
{
    if(!state_ptr)
    {
        return false;
    }
    return state_ptr->keyboard_current.keys[key] == false;
}

b8 input_was_key_up(key_code key)
{
    if(!state_ptr)
    {
        return false;
    }
    return state_ptr->keyboard_previous.keys[key] == false;
}

b8 input_is_button_down(mouse_button button)
{
    if(!state_ptr)
    {
        return false;
    }
    return state_ptr->mousebutton_current.buttons[button] == true;
}

b8 input_was_button_down(mouse_button button)
{
    if(!state_ptr)
    {
        return false;
    }
    return state_ptr->mousebutton_previous.buttons[button] == true;
}

b8 input_is_button_up(mouse_button button)
{
    if(!state_ptr)
    {
        return false;
    }
    return state_ptr->mousebutton_current.buttons[button] == false;
}

b8 input_was_button_up(mouse_button button)
{
    if(!state_ptr)
    {
        return false;
    }
    return state_ptr->mousebutton_previous.buttons[button] == false;
}

void input_get_mouse_pos(i32* x, i32* y)
{
    if(!state_ptr)
    {
        *x = 0;
        *y = 0;
    }
    *x = state_ptr->mousebutton_current.x;
    *y = state_ptr->mousebutton_current.y;
}

void input_get_prev_mouse_pos(i32* x, i32* y)
{
    if(!state_ptr)
    {
        *x = 0;
        *y = 0;
    }
    *x = state_ptr->mousebutton_previous.x;
    *y = state_ptr->mousebutton_previous.y;
}